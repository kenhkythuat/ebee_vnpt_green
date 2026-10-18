/*
 * common_modbus_rs485.h
 *
 *  Created on: Sep 17, 2025
 *      Author: MY PC
 */

#include "stdbool.h"
#include <stdint.h>
#include <config.h>
#ifndef INC_COMMON_MODBUS_RS485_H_
#define INC_COMMON_MODBUS_RS485_H_

extern char rx_buffer_sensor[30];
uint16_t modbus_crc16(uint8_t *buf, uint16_t len);
extern void read_sensor(void);

#if SM7820_RS485
extern float data_air_temperature;
extern float data_air_humidity;
#endif

// number address is 11
#if LX200K_485
extern float data_light_lx200k;
#endif

#if PT100_01
extern float temperature_pt100_1;
#endif

#if PT100_02
extern float temperature_pt100_2;
#endif

#if YW001
extern float water_level;
#endif

#if RK500_22
extern float data_ph_rk500_22;
#endif

#if RK520_01
extern float data_soil_temperature_rika520_01;
extern float data_soil_moisture_rika520_01;
#endif

#if NBL_S_TMC_7
extern float data_soil_temperature_niubol_7;
extern float data_soil_moisture_niubol_7;
extern float data_soil_conductivity_niubol_7;
extern float data_soil_pH_niubol_7;
extern float data_soil_nitrogen_niubol_7;
extern float data_soil_phospho_niubol_7;
extern float data_soil_kali_niubol_7;
#endif

#if NBL_S_NPK
extern float data_soil_nitrogen_NBL_S_NPK;
extern float data_soil_phospho__NBL_S_NPK;
extern float data_soil_kali__NBL_S_NPK;

#endif

#if CO2_UART
extern int data_co2;
#endif


#endif /* INC_COMMON_MODBUS_RS485_H_ */
