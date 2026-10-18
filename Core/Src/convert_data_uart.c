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

      if (strstr(rx_buffer_simcom, "ota_check") != NULL ||
          strstr(rx_buffer_simcom, "ota_update") != NULL) {
        printf("--------------OTA request received---------------\r\n");
        to_start_ota = true;
      }
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
