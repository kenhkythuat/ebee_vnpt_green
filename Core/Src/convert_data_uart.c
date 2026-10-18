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

static void reset_mqtt_rx_window(void) {
  mqtt_rx_window_length = 0U;
  mqtt_rx_window[0] = '\0';
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
    printf("--------------ThingsBoard OTA request received---------------\r\n");
    to_start_ota = true;
    reset_mqtt_rx_window();
    return;
  }

  if (strstr(mqtt_rx_window, "+CMQTTRXEND:") != NULL) {
    reset_mqtt_rx_window();
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart->Instance == USART1) {
      uint16_t text_size = Size;

      if (ota_uart_rx_capture((const uint8_t *)rx_buffer_simcom, Size)) {
        memset(rx_buffer_simcom, '\0', sizeof(rx_buffer_simcom));
        HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t *)rx_buffer_simcom,
                                   sizeof(rx_buffer_simcom));
        return;
      }
      if (text_size >= sizeof(rx_buffer_simcom)) {
        text_size = sizeof(rx_buffer_simcom) - 1U;
      }
      rx_buffer_simcom[text_size] = '\0';
      printf("\r\nSIMCOM Response:");
      printf("%s", rx_buffer_simcom);
      memcpy(rx_data_sim, rx_buffer_simcom, text_size + 1U);
      handle_mqtt_ota_message(rx_buffer_simcom, text_size);
      memset(rx_buffer_simcom, '\0', sizeof(rx_buffer_simcom));
  }
  if (huart->Instance == USART2){
      printf("\r\nUART2 Response:");
      for (uint16_t i = 0; i < Size; i++) {
        printf(" %02X", (uint8_t)rx_uart2[i]);
      }
      printf("\r\n");
  }
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t *)rx_buffer_simcom, 700);
  HAL_UARTEx_ReceiveToIdle_IT(&huart4, (uint8_t *)rx_buffer_sensor, 30);
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t *)rx_uart2, 9);
 }
