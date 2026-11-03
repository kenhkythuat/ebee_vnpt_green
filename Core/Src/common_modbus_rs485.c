/*
 * common_modbus_rs485.c
 *
 *  Created on: Sep 17, 2025
 *      Author: MY PC
 */
#include "common_modbus_rs485.h"
#include "stdio.h"
#include "cJSON.h"
#include "config.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <main.h>
#include <math.h>

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
char rx_buffer_sensor[30];
char rx_buffer_sensor_uart[30];
volatile uint16_t rx_buffer_sensor_length;
static volatile bool rx_buffer_sensor_ready;

#define MODBUS_READ_ATTEMPTS 2U
#define MODBUS_RESPONSE_TIMEOUT_MS 700U
#define MODBUS_RETRY_DELAY_MS 100U

// data PH Fuvitech
float data_ph_fuvitech = NAN;
float data_measured_ph_fuvitech = NAN;
float data_temperature_ph_fuvitech = NAN;
// data EC Fuvitech
float data_ec_fuvitech = NAN;
float data_conductivity_ec_fuvitech = NAN;
float data_resistivity_ec_fuvitech = NAN;
float data_temperateure_ec_fuvitech = NAN;
float data_tds_ec_fuvitech = NAN;
float data_salinity_ec_fuvitech = NAN;

// data DO Fuvitech
float data_do_fuvitech = NAN;
float data_dissolved_oxygen_fuvitech = NAN;
float data_temperature_do_fuvitech = NAN;

// number address is 1
#if RK520_01
unsigned char command_rika520_01[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
float data_soil_temperature_rika520_01 = NAN;
float data_soil_moisture_rika520_01 = NAN;
uint8_t respone_rk520_01 = false;
#endif

// number address is 2
#if RK520_02
unsigned char command_rika520_02[8] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x03, 0x05, 0xF8};
float data_soil_temperature_rika520_02 = NAN;
float data_soil_moisture_rika520_02 = NAN;
float EC_rk520_02 = NAN;
uint8_t respone_rk520_02 = false;
#endif

#if NBL_S_TMC_7
// unsigned char command_niubol_7in1[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08};
unsigned char command_niubol_7in1[8] = {0x07, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x6E};
float data_soil_temperature_niubol_7 = NAN;
float data_soil_moisture_niubol_7 = NAN;
float data_soil_conductivity_niubol_7 = NAN;
float data_soil_pH_niubol_7 = NAN;
float data_soil_nitrogen_niubol_7 = NAN;
float data_soil_phospho_niubol_7 = NAN;
float data_soil_kali_niubol_7 = NAN;
uint8_t respone_niubol_7in1 = false;
#endif

// number address is 3
#if NBL_S_NPK
// unsigned char data_nbl_s_npk[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x03, 0x05, 0xCB};
unsigned char command_nbl_s_npk[8] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x04, 0x29};
float data_soil_nitrogen_NBL_S_NPK = NAN;
float data_soil_phospho__NBL_S_NPK = NAN;
float data_soil_kali__NBL_S_NPK = NAN;

uint8_t respone_nbl_s_npk = false;
#endif

// number address is 5
#if rk500_02
unsigned char rika500_02[8] = {0x05, 0x03, 0x00, 0x00, 0x00, 0x01, 0x85, 0x8E};
float data_soil_pH = NAN;
uint8_t respone_rk500_02 = false;
#endif

#if do_fuvitech
unsigned char command_do_fuvitech[8] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x08, 0x45, 0xC9};
#endif

#if ph_fuvitech
// unsigned char measured_command_ph_fuvitech[8] = {0x02, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xF8};
// unsigned char temperature_command_ph_fuvitech[8] = {0x02, 0x03, 0x00, 0x03, 0x00, 0x02, 0x34, 0x38};

unsigned char measured_command_ph_fuvitech[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB};
unsigned char temperature_command_ph_fuvitech[8] = {0x01, 0x03, 0x00, 0x03, 0x00, 0x02, 0x34, 0x0B};
#endif

#if ec_fuvitech
// unsigned char conductivity_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xE9};
// unsigned char resistivity_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x02, 0x00, 0x02, 0x64, 0x29};
// unsigned char temperateure_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x04, 0x00, 0x02, 0x84, 0x28};
// unsigned char tds_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x06, 0x00, 0x02, 0x25, 0xE8};
// unsigned char salinity_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x08, 0x00, 0x02, 0x44, 0x2B};

unsigned char conductivity_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
unsigned char resistivity_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xCB};
unsigned char temperateure_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x04, 0x00, 0x02, 0x85, 0xCA};
unsigned char tds_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x02, 0x24, 0x0A};
unsigned char salinity_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x08, 0x00, 0x02, 0x45, 0xC9};

#endif
// number address is 11
#if SM7820_RS485
unsigned char command_sm7820b[8] = {0x0B, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0xA1};
uint8_t respone_sm7820b = false;
float data_air_temperature = NAN;
float data_air_humidity = NAN;

#endif

// number address is 10
#if LX200K_485
unsigned char command_lx200k_485[8] = {0x0A, 0x03, 0x00, 0x02, 0x00, 0x02, 0x64, 0xB0};
float data_light_lx200k = NAN;
uint8_t respone_lx200k_485 = false;
#endif

// PT100 register 0x0001 (Modbus start address 0x0000), device address 12
#if PT100_01
unsigned char command_pt100_01[8] = {0x0C, 0x03, 0x00, 0x00, 0x00, 0x01, 0x85, 0x17};
float temperature_pt100_1 = NAN;
#endif

// PT100 register 0x0001 (Modbus start address 0x0000), device address 13
#if PT100_02
unsigned char command_pt100_02[8] = {0x0D, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0xC6};
float temperature_pt100_2 = NAN;
#endif

// YW001 pressure and decimal places, device address 14
#if YW001
unsigned char command_yw001[8] = {0x0E, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0x34};
float water_level = NAN;
#endif

#if RK500_22
unsigned char command_rk500_22[8] = {0x06, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xBC};
// unsigned char command_rk500_22[8] = { 0x03, 0x06, 0x00, 0x14, 0x00, 0x06, 0x48, 0x2E };
float data_ph_rk500_22 = NAN;
uint8_t respone_ph_rk500_22 = false;
#endif

#if CO2_UART
unsigned char command_co2[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
uint8_t rx_co2[10];
int data_co2 = -1;
bool data_co2_valid = false;
uint8_t respone_co2 = false;
#endif

uint16_t modbus_crc16(uint8_t *buf, uint16_t len)
{
  uint16_t crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++)
  {
    crc ^= buf[pos];
    for (int i = 0; i < 8; i++)
    {
      if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc >>= 1;
    }
  }
  return crc;
}

static bool modbus_response_is_valid(const uint8_t *request)
{
  const uint16_t register_count = ((uint16_t)request[4] << 8) | request[5];
  const uint16_t byte_count = register_count * 2U;
  const uint16_t expected_length = byte_count + 5U;
  uint16_t received_crc;
  uint16_t calculated_crc;

  if (register_count == 0U || expected_length > sizeof(rx_buffer_sensor) ||
      rx_buffer_sensor_length < expected_length ||
      (uint8_t)rx_buffer_sensor[0] != request[0] ||
      (uint8_t)rx_buffer_sensor[1] != request[1] ||
      (uint8_t)rx_buffer_sensor[2] != byte_count)
  {
    return false;
  }

  calculated_crc = modbus_crc16((uint8_t *)rx_buffer_sensor,
                                expected_length - 2U);
  received_crc = (uint16_t)(uint8_t)rx_buffer_sensor[expected_length - 2U] |
                 ((uint16_t)(uint8_t)rx_buffer_sensor[expected_length - 1U]
                  << 8);
  return calculated_crc == received_crc;
}

void modbus_rs485_rx_event(uint16_t size)
{
  uint16_t available;
  uint16_t copy_length;
  uint16_t expected_length;

  /* UART4 receives into a separate interrupt buffer. Once a complete Modbus
   * frame has been assembled, keep the processing buffer unchanged until the
   * foreground code has validated and decoded it. */
  if (size == 0U || rx_buffer_sensor_ready)
  {
    return;
  }

  available = sizeof(rx_buffer_sensor) - rx_buffer_sensor_length;
  copy_length = size < available ? size : available;
  if (copy_length == 0U)
  {
    return;
  }

  memcpy(&rx_buffer_sensor[rx_buffer_sensor_length], rx_buffer_sensor_uart,
         copy_length);
  rx_buffer_sensor_length += copy_length;

  if (rx_buffer_sensor_length < 3U)
  {
    return;
  }

  if (((uint8_t)rx_buffer_sensor[1] & 0x80U) != 0U)
  {
    expected_length = 5U; /* Modbus exception response. */
  }
  else
  {
    expected_length = (uint16_t)(uint8_t)rx_buffer_sensor[2] + 5U;
  }

  if (expected_length <= sizeof(rx_buffer_sensor) &&
      rx_buffer_sensor_length >= expected_length)
  {
    rx_buffer_sensor_ready = true;
  }
}

float modbus_bytes_to_float(char *data, int offset)
{
  uint8_t reordered[4] = {
      data[offset + 0], // B
      data[offset + 1], // A
      data[offset + 2], // D
      data[offset + 3]  // C
  };

  uint32_t raw = (reordered[0] << 24) | (reordered[1] << 16) | (reordered[2] << 8) | reordered[3];
  float value;
  memcpy(&value, &raw, sizeof(value));
  return value;
}

#if PT100_01 || PT100_02
static float pt100_response_to_temperature(const char *response)
{
  int16_t raw_temperature = (int16_t)(((uint16_t)(uint8_t)response[3] << 8) |
                                      (uint16_t)(uint8_t)response[4]);
  return (float)raw_temperature / 10.0f;
}
#endif

#if YW001
static bool yw001_response_to_water_level_cm(const char *response, float *water_level_cm)
{
  const uint16_t calculated_crc = modbus_crc16((uint8_t *)response, 7);
  const uint16_t received_crc = (uint16_t)(uint8_t)response[7] |
                                ((uint16_t)(uint8_t)response[8] << 8);

  if ((uint8_t)response[0] != 0x0E ||
      (uint8_t)response[1] != 0x04 ||
      (uint8_t)response[2] != 0x04 ||
      calculated_crc != received_crc)
  {
    return false;
  }

  const int16_t raw_pressure = (int16_t)(((uint16_t)(uint8_t)response[3] << 8) |
                                         (uint16_t)(uint8_t)response[4]);
  const uint16_t decimal_places = ((uint16_t)(uint8_t)response[5] << 8) |
                                  (uint16_t)(uint8_t)response[6];

  if (decimal_places > 6)
  {
    return false;
  }

  float divisor = 1.0f;
  for (uint16_t i = 0; i < decimal_places; i++)
  {
    divisor *= 10.0f;
  }

  // The sensor reports mH2O. Convert it to centimetres of water.
  *water_level_cm = ((float)raw_pressure / divisor) * 100.0f;
  return true;
}
#endif

bool send_data_to_rs485(uint8_t *data, char *data_log)
{
  uint32_t started_at;

  printf("Read measured form %s\r\n", data_log);
  for (uint8_t attempt = 1U; attempt <= MODBUS_READ_ATTEMPTS; attempt++)
  {
    /* Stop any old/incomplete reception before starting a new transaction. */
    HAL_UART_AbortReceive(&huart4);
    __HAL_UART_CLEAR_OREFLAG(&huart4);
    memset(rx_buffer_sensor, 0, sizeof(rx_buffer_sensor));
    memset(rx_buffer_sensor_uart, 0, sizeof(rx_buffer_sensor_uart));
    rx_buffer_sensor_length = 0U;
    rx_buffer_sensor_ready = false;

    if (HAL_UARTEx_ReceiveToIdle_IT(&huart4,
                                   (uint8_t *)rx_buffer_sensor_uart,
                                   sizeof(rx_buffer_sensor_uart)) != HAL_OK)
    {
      printf("[MODBUS] %s RX start failed attempt=%u\r\n", data_log,
             attempt);
    }
    else if (HAL_UART_Transmit(&huart4, data, 8, 1000) != HAL_OK)
    {
      printf("[MODBUS] %s transmit failed attempt=%u\r\n", data_log,
             attempt);
    }
    else
    {
      started_at = HAL_GetTick();
      while (!rx_buffer_sensor_ready &&
             (HAL_GetTick() - started_at) < MODBUS_RESPONSE_TIMEOUT_MS)
      {
        HAL_Delay(1);
      }

      if (rx_buffer_sensor_ready && modbus_response_is_valid(data))
      {
        return true;
      }

      printf("[MODBUS] %s invalid/timeout attempt=%u length=%u\r\n",
             data_log, attempt, rx_buffer_sensor_length);
    }

    if (attempt < MODBUS_READ_ATTEMPTS)
    {
      HAL_Delay(MODBUS_RETRY_DELAY_MS);
    }
  }

  return false;
}

bool send_data_to_(uint8_t *data, char *data_log)
{
  return send_data_to_rs485(data, data_log);
}

#if CO2_UART
static bool co2_response_is_valid(void)
{
  uint8_t checksum = 0U;

  if (rx_uart2_length < 9U || (uint8_t)rx_uart2[0] != 0xFFU ||
      (uint8_t)rx_uart2[1] != 0x86U)
  {
    return false;
  }

  for (uint8_t index = 1U; index <= 7U; index++)
  {
    checksum = (uint8_t)(checksum + (uint8_t)rx_uart2[index]);
  }
  checksum = (uint8_t)(0xFFU - checksum + 1U);
  return checksum == (uint8_t)rx_uart2[8];
}

static bool send_data_co2(uint8_t *data, char *data_log)
{
  printf("Read data CO2 form %s\r\n", data_log);
  memset(rx_uart2, '\0', sizeof(rx_uart2));
  rx_uart2_length = 0U;
  HAL_Delay(100);
  if (HAL_UART_Transmit(&huart2, data, 9, 1000) != HAL_OK)
  {
    printf("[CO2] UART transmit failed\r\n");
    return false;
  }
  HAL_Delay(500);
  if (!co2_response_is_valid())
  {
    printf("[CO2] no/invalid response length=%u\r\n", rx_uart2_length);
    return false;
  }
  return true;
}
#endif

void read_sensor(void)
{
  // read PH Fuvitech
#if ph_fuvitech
    data_measured_ph_fuvitech =
        send_data_to_rs485(measured_command_ph_fuvitech, "PH")
            ? modbus_bytes_to_float(rx_buffer_sensor, 3) : NAN;
    data_temperature_ph_fuvitech =
        send_data_to_rs485(temperature_command_ph_fuvitech, "PH")
            ? modbus_bytes_to_float(rx_buffer_sensor, 3) : NAN;
#endif
    // read EC Fuvitech
#if ec_fuvitech
    data_conductivity_ec_fuvitech =
        send_data_to_rs485(conductivity_command_ec_fuvitech, "EC")
            ? modbus_bytes_to_float(rx_buffer_sensor, 3) * 1000.0f : NAN;
    data_resistivity_ec_fuvitech =
        send_data_to_rs485(resistivity_command_ec_fuvitech, "EC")
            ? modbus_bytes_to_float(rx_buffer_sensor, 3) : NAN;
    data_temperateure_ec_fuvitech =
        send_data_to_rs485(temperateure_command_ec_fuvitech, "EC")
            ? modbus_bytes_to_float(rx_buffer_sensor, 3) : NAN;
    data_tds_ec_fuvitech =
        send_data_to_rs485(tds_command_ec_fuvitech, "EC")
            ? modbus_bytes_to_float(rx_buffer_sensor, 3) : NAN;
    data_salinity_ec_fuvitech = isfinite(data_tds_ec_fuvitech)
                                    ? data_tds_ec_fuvitech / 640.0f : NAN;
#endif
#if do_fuvitech
    data_dissolved_oxygen_fuvitech =
        send_data_to_rs485(command_do_fuvitech, "DO")
            ? modbus_bytes_to_float(rx_buffer_sensor, 3) / 100.0f : NAN;
#endif
#if SM7820_RS485
    if (send_data_to_rs485(command_sm7820b, "SM7820"))
    {
      data_air_temperature =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[3] << 8) |
                  (uint8_t)rx_buffer_sensor[4]) / 100.0f;
      data_air_humidity =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[5] << 8) |
                  (uint8_t)rx_buffer_sensor[6]) / 100.0f;
      printf("data_air_temperature %.2f\r\n", data_air_temperature);
      printf("data_air_humidity %.2f\r\n", data_air_humidity);
    }
    else
    {
      data_air_temperature = NAN;
      data_air_humidity = NAN;
    }
    HAL_Delay(1000);
#endif
#if LX200K_485
    if (send_data_to_rs485(command_lx200k_485, "LK200K"))
    {
      uint32_t raw_light = ((uint32_t)(uint8_t)rx_buffer_sensor[3] << 24) |
                           ((uint32_t)(uint8_t)rx_buffer_sensor[4] << 16) |
                           ((uint32_t)(uint8_t)rx_buffer_sensor[5] << 8) |
                           (uint8_t)rx_buffer_sensor[6];
      data_light_lx200k = (float)raw_light / 1000.0f;
      printf("data_light_sensor %.2f\r\n", data_light_lx200k);
    }
    else
    {
      data_light_lx200k = NAN;
    }
    HAL_Delay(1000);
#endif

#if PT100_01
    if (send_data_to_rs485(command_pt100_01, "PT100_01"))
    {
      temperature_pt100_1 = pt100_response_to_temperature(rx_buffer_sensor);
      printf("temperature_pt100_1 %.1f\r\n", temperature_pt100_1);
    }
    else
    {
      temperature_pt100_1 = NAN;
    }
    HAL_Delay(1000);
#endif

#if PT100_02
    if (send_data_to_rs485(command_pt100_02, "PT100_02"))
    {
      temperature_pt100_2 = pt100_response_to_temperature(rx_buffer_sensor);
      printf("temperature_pt100_2 %.1f\r\n", temperature_pt100_2);
    }
    else
    {
      temperature_pt100_2 = NAN;
    }
    HAL_Delay(1000);
#endif

#if YW001
    if (send_data_to_rs485(command_yw001, "YW001") &&
        yw001_response_to_water_level_cm(rx_buffer_sensor, &water_level))
    {
      printf("water_level %.1f cmH2O\r\n", water_level);
    }
    else
    {
      water_level = NAN;
      printf("Invalid YW001 Modbus response\r\n");
    }
    HAL_Delay(1000);
#endif

#if RK500_22
    if (send_data_to_rs485(command_rk500_22, "RK500_22"))
    {
      data_ph_rk500_22 = modbus_bytes_to_float(rx_buffer_sensor, 3);
      printf("data_soil_ph %.2f\r\n", data_ph_rk500_22);
    }
    else
    {
      data_ph_rk500_22 = NAN;
    }
    HAL_Delay(1000);
#endif

#if RK520_01
    if (send_data_to_rs485(command_rika520_01, "RK520_01"))
    {
      data_soil_temperature_rika520_01 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[3] << 8) |
                  (uint8_t)rx_buffer_sensor[4]) * 0.1f;
      data_soil_moisture_rika520_01 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[5] << 8) |
                  (uint8_t)rx_buffer_sensor[6]) * 0.1f;
    }
    else
    {
      data_soil_temperature_rika520_01 = NAN;
      data_soil_moisture_rika520_01 = NAN;
    }
    HAL_Delay(1000);
#endif

#if RK520_02
    if (send_data_to_rs485(command_rika520_02, "RK520_02"))
    {
      data_soil_temperature_rika520_02 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[3] << 8) |
                  (uint8_t)rx_buffer_sensor[4]);
      data_soil_moisture_rika520_02 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[5] << 8) |
                  (uint8_t)rx_buffer_sensor[6]);
    }
    else
    {
      data_soil_temperature_rika520_02 = NAN;
      data_soil_moisture_rika520_02 = NAN;
      EC_rk520_02 = NAN;
    }
    HAL_Delay(1000);
#endif

#if NBL_S_TMC_7
    if (send_data_to_rs485(command_niubol_7in1, "NIUBOL 7IN1"))
    {
      data_soil_temperature_niubol_7 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[3] << 8) |
                  (uint8_t)rx_buffer_sensor[4]) * 0.1f;
      data_soil_moisture_niubol_7 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[5] << 8) |
                  (uint8_t)rx_buffer_sensor[6]) * 0.1f;
      data_soil_conductivity_niubol_7 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[7] << 8) |
                  (uint8_t)rx_buffer_sensor[8]);
      data_soil_pH_niubol_7 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[9] << 8) |
                  (uint8_t)rx_buffer_sensor[10]) * 0.01f;
      float temp_pH_niubol_7 = data_soil_pH_niubol_7;
      if (6.51f < temp_pH_niubol_7 && temp_pH_niubol_7 < 7.0f)
      {
        data_soil_pH_niubol_7 -= 0.8f;
      }
      else if (7.01f < temp_pH_niubol_7 && temp_pH_niubol_7 < 8.0f)
      {
        data_soil_pH_niubol_7 -= 1.5f;
      }
      else if (8.01f < temp_pH_niubol_7 && temp_pH_niubol_7 < 8.5f)
      {
        data_soil_pH_niubol_7 -= 2.0f;
      }
      data_soil_nitrogen_niubol_7 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[11] << 8) |
                  (uint8_t)rx_buffer_sensor[12]);
      data_soil_phospho_niubol_7 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[13] << 8) |
                  (uint8_t)rx_buffer_sensor[14]);
      data_soil_kali_niubol_7 =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[15] << 8) |
                  (uint8_t)rx_buffer_sensor[16]);
    }
    else
    {
      data_soil_temperature_niubol_7 = NAN;
      data_soil_moisture_niubol_7 = NAN;
      data_soil_conductivity_niubol_7 = NAN;
      data_soil_pH_niubol_7 = NAN;
      data_soil_nitrogen_niubol_7 = NAN;
      data_soil_phospho_niubol_7 = NAN;
      data_soil_kali_niubol_7 = NAN;
    }
    HAL_Delay(1000);
#endif

#if NBL_S_NPK
    if (send_data_to_rs485(command_nbl_s_npk, "NIUBOL_S_NPK"))
    {
      data_soil_nitrogen_NBL_S_NPK =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[3] << 8) |
                  (uint8_t)rx_buffer_sensor[4]);
      data_soil_phospho__NBL_S_NPK =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[5] << 8) |
                  (uint8_t)rx_buffer_sensor[6]);
      data_soil_kali__NBL_S_NPK =
          (float)(((uint16_t)(uint8_t)rx_buffer_sensor[7] << 8) |
                  (uint8_t)rx_buffer_sensor[8]);
    }
    else
    {
      data_soil_nitrogen_NBL_S_NPK = NAN;
      data_soil_phospho__NBL_S_NPK = NAN;
      data_soil_kali__NBL_S_NPK = NAN;
    }
    HAL_Delay(1000);
#endif
#if CO2_UART
    if (send_data_co2(command_co2, "CO2"))
    {
      data_co2 = ((int)(uint8_t)rx_uart2[2] << 8) |
                 (uint8_t)rx_uart2[3];
      data_co2_valid = true;
    }
    else
    {
      data_co2 = -1;
      data_co2_valid = false;
    }
    HAL_Delay(100);
#endif
}
