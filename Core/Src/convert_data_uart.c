/*
 * convert_data_uart.c
 *
 *  Created on: Sep 17, 2025
 *      Author: MY PC
 */
#include "cJSON.h"
#include "config.h"
#include "main.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>
#include "convert_data_uart.h"
#include "common_simcom.h"
#include "ota_update.h"

#define MQTT_RX_WINDOW_SIZE 768U

static char mqtt_rx_window[MQTT_RX_WINDOW_SIZE];
static size_t mqtt_rx_window_length;
static bool mqtt_ota_command_seen;

static HAL_StatusTypeDef start_receive_to_idle(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    return HAL_UARTEx_ReceiveToIdle_IT(huart, (uint8_t *)rx_buffer_simcom,
                                      sizeof(rx_buffer_simcom));
  }
  if (huart->Instance == USART2) {
    return HAL_UARTEx_ReceiveToIdle_IT(huart, (uint8_t *)rx_uart2,
                                      sizeof(rx_uart2) - 1U);
  }
  if (huart->Instance == UART4) {
    return HAL_UARTEx_ReceiveToIdle_IT(huart, (uint8_t *)rx_buffer_sensor,
                                      sizeof(rx_buffer_sensor));
  }
  return HAL_ERROR;
}

static void restart_receive_to_idle(UART_HandleTypeDef *huart) {
  HAL_StatusTypeDef status = start_receive_to_idle(huart);

  if (status == HAL_OK) {
    return;
  }
  if (huart->Instance == USART1) {
    ota_uart_rx_error(0x80000000UL | (uint32_t)status);
  }

  /* A failed re-arm normally means the HAL receive state did not return to
   * READY. Reset only this UART RX path, clear stale error flags, and retry. */
  HAL_UART_AbortReceive(huart);
  __HAL_UART_CLEAR_OREFLAG(huart);
  status = start_receive_to_idle(huart);
  if (status != HAL_OK && huart->Instance == USART1) {
    ota_uart_rx_error(0x40000000UL | (uint32_t)status);
  }
}

static void reset_mqtt_rx_window(void) {
  mqtt_rx_window_length = 0U;
  mqtt_rx_window[0] = '\0';
  mqtt_ota_command_seen = false;
}

static void append_mqtt_rx_window(const char *data, size_t length) {
  size_t capacity = sizeof(mqtt_rx_window) - 1U;

  if (length >= capacity) {
    data += length - capacity;
    length = capacity;
    mqtt_rx_window_length = 0U;
  } else if ((mqtt_rx_window_length + length) > capacity) {
    size_t remove_length = mqtt_rx_window_length + length - capacity;
    memmove(mqtt_rx_window, mqtt_rx_window + remove_length,
            mqtt_rx_window_length - remove_length);
    mqtt_rx_window_length -= remove_length;
  }

  memcpy(mqtt_rx_window + mqtt_rx_window_length, data, length);
  mqtt_rx_window_length += length;
  mqtt_rx_window[mqtt_rx_window_length] = '\0';
}

static void handle_mqtt_ota_message(const char *data, size_t length) {
  static const char ota_payload[] = "{\"OTA\":\"ota_check\"}";

  if (strstr(data, "+CMQTTRXSTART:") != NULL) {
    reset_mqtt_rx_window();
  }

  append_mqtt_rx_window(data, length);
  if (strstr(mqtt_rx_window, MQTT_TOPIC_ATTRIBUTES_THINGSBOARD) != NULL &&
      strstr(mqtt_rx_window, ota_payload) != NULL) {
    mqtt_ota_command_seen = true;
  }

  if (strstr(mqtt_rx_window, "+CMQTTRXEND:") != NULL) {
    if (mqtt_ota_command_seen) {
      to_start_ota = true;
    }
    reset_mqtt_rx_window();
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart->Instance == USART1) {
      uint16_t text_size = Size;

      if (ota_uart_rx_capture((const uint8_t *)rx_buffer_simcom, Size)) {
        restart_receive_to_idle(huart);
        return;
      }
      if (text_size >= sizeof(rx_buffer_simcom)) {
        text_size = sizeof(rx_buffer_simcom) - 1U;
      }
      rx_buffer_simcom[text_size] = '\0';
      memcpy(rx_data_sim, rx_buffer_simcom, text_size + 1U);
      handle_mqtt_ota_message(rx_buffer_simcom, text_size);
      rx_buffer_simcom[0] = '\0';
      restart_receive_to_idle(huart);
      return;
  }
  if (huart->Instance == USART2){
      if (!ota_non_modem_io_paused) {
        restart_receive_to_idle(huart);
      }
      return;
  }
  if (huart->Instance == UART4) {
    if (!ota_non_modem_io_paused) {
      restart_receive_to_idle(huart);
    }
  }
 }

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  uint32_t error_code = HAL_UART_GetError(huart);

  if (huart->Instance == USART1) {
    ota_uart_rx_error(error_code);
  }

  if ((huart->Instance == USART2 || huart->Instance == UART4) &&
      ota_non_modem_io_paused) {
    HAL_UART_AbortReceive(huart);
    __HAL_UART_CLEAR_OREFLAG(huart);
    return;
  }

  /* Abort clears the HAL RX state after framing/noise/overrun errors. The
   * receive-to-idle channel is then restored immediately without printing or
   * doing other slow work in interrupt context. */
  HAL_UART_AbortReceive(huart);
  __HAL_UART_CLEAR_OREFLAG(huart);
  restart_receive_to_idle(huart);
}
