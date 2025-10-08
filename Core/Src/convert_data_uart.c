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



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart->Instance == USART1) {
	      printf("\r\nSIMCOM Response:");
	      printf(rx_buffer_simcom);
	      for (int i = 0; i < 700; i++) {
	        rx_data_sim[i] = rx_buffer_simcom[i];
	      }
  }
  memset(rx_buffer_simcom, '\0', 700);
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t *)rx_buffer_simcom, 700);
  HAL_UARTEx_ReceiveToIdle_IT(&huart4, (uint8_t *)rx_buffer_sensor, 30);
 }
