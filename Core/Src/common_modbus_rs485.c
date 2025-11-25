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

extern UART_HandleTypeDef huart4;
char rx_buffer_sensor[30];

// data PH Fuvitech
float data_ph_fuvitech;
float data_measured_ph_fuvitech;
float data_temperature_ph_fuvitech;
// data EC Fuvitech
float data_ec_fuvitech;
float data_conductivity_ec_fuvitech;
float data_resistivity_ec_fuvitech;
float data_temperateure_ec_fuvitech;
float data_tds_ec_fuvitech;
float data_salinity_ec_fuvitech;

// data DO Fuvitech
float data_do_fuvitech;
float data_dissolved_oxygen_fuvitech;
float data_temperature_do_fuvitech;

// number address is 1
#if RK520_01
	unsigned char command_rika520_01[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
	float data_soil_temperature_rika520_01;
	float data_soil_moisture_rika520_01;
	uint8_t respone_rk520_01= false;
#endif

// number address is 2
#if RK520_02
unsigned char command_rika520_02[8] = { 0x02, 0x03, 0x00, 0x00, 0x00, 0x03, 0x05, 0xF8 };
float data_soil_temperature_rika520_02;
float data_soil_moisture_rika520_02;
float EC_rk520_02 = 0;
uint8_t respone_rk520_02 = false;
#endif

#if NBL_S_TMC_7
//unsigned char command_niubol_7in1[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08};
unsigned char command_niubol_7in1[8] = {0x07, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x6E};
float data_soil_temperature_niubol_7=0;
float data_soil_moisture_niubol_7=0;
float data_soil_conductivity_niubol_7=0;
float data_soil_pH_niubol_7=0;
float data_soil_nitrogen_niubol_7=0;
float data_soil_phospho_niubol_7=0;
float data_soil_kali_niubol_7=0;
uint8_t respone_niubol_7in1 = false;
#endif

// number address is 3
#if NBL_S_NPK
//unsigned char data_nbl_s_npk[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x03, 0x05, 0xCB};
unsigned char command_nbl_s_npk[8] = { 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x04, 0x29 };
float data_soil_nitrogen_NBL_S_NPK=0;
float data_soil_phospho__NBL_S_NPK=0;
float data_soil_kali__NBL_S_NPK=0;

uint8_t respone_nbl_s_npk = false;
#endif

// number address is 5
#if rk500_02
unsigned char rika500_02[8] = { 0x05, 0x03, 0x00, 0x00, 0x00, 0x01, 0x85, 0x8E };
float data_soil_pH = 0;
uint8_t respone_rk500_02 = false;
#endif

#if do_fuvitech
unsigned char command_do_fuvitech[8] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x08, 0x45, 0xC9};
#endif

#if ph_fuvitech
//unsigned char measured_command_ph_fuvitech[8] = {0x02, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xF8};
//unsigned char temperature_command_ph_fuvitech[8] = {0x02, 0x03, 0x00, 0x03, 0x00, 0x02, 0x34, 0x38};

unsigned char measured_command_ph_fuvitech[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB};
unsigned char temperature_command_ph_fuvitech[8] = {0x01, 0x03, 0x00, 0x03, 0x00, 0x02, 0x34, 0x0B};
#endif

#if ec_fuvitech
//unsigned char conductivity_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xE9};
//unsigned char resistivity_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x02, 0x00, 0x02, 0x64, 0x29};
//unsigned char temperateure_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x04, 0x00, 0x02, 0x84, 0x28};
//unsigned char tds_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x06, 0x00, 0x02, 0x25, 0xE8};
//unsigned char salinity_command_ec_fuvitech[8] = {0x03, 0x03, 0x00, 0x08, 0x00, 0x02, 0x44, 0x2B};

unsigned char conductivity_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
unsigned char resistivity_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xCB};
unsigned char temperateure_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x04, 0x00, 0x02, 0x85, 0xCA};
unsigned char tds_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x02, 0x24, 0x0A};
unsigned char salinity_command_ec_fuvitech[8] = {0x01, 0x03, 0x00, 0x08, 0x00, 0x02, 0x45, 0xC9};

#endif
// number address is 12
#if SM7820_RS485
unsigned char command_sm7820b[8] = { 0x0B, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0xA1 };
uint8_t respone_sm7820b = false;
float data_air_temperature;
float data_air_humidity;

#endif

// number address is 11
#if LX200K_485
unsigned char command_lx200k_485[8] = { 0x0A, 0x03, 0x00, 0x02, 0x00, 0x02, 0x64, 0xB0 };
float data_light_lx200k=0;
uint8_t respone_lx200k_485 = false;
#endif

#if RK500_22
unsigned char command_rk500_22[8] = { 0x06, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xBC };
//unsigned char command_rk500_22[8] = { 0x03, 0x06, 0x00, 0x14, 0x00, 0x06, 0x48, 0x2E };
float data_ph_rk500_22=0;
uint8_t respone_ph_rk500_22 = false;
#endif

uint16_t modbus_crc16(uint8_t *buf, uint16_t len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= buf[pos];
        for (int i = 0; i < 8; i++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

float modbus_bytes_to_float( char *data, int offset)
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

float send_data_to_rs485(uint8_t *data, char * data_log) {
  printf("Read measured form %s\r\n",data_log);
  memset(rx_buffer_sensor, '\0', 30);
  HAL_Delay(100);
  HAL_UART_Transmit(&huart4, data, 8, 1000);
  HAL_Delay(700);
  return 1;;
}
void read_sensor(void){
  // read PH Fuvitech
#if ph_fuvitech
  data_measured_ph_fuvitech = send_data_to_rs485(measured_command_ph_fuvitech,"PH");
  data_temperature_ph_fuvitech = send_data_to_rs485(temperature_command_ph_fuvitech,"PH");
#endif
  // read EC Fuvitech
#if ec_fuvitech
  data_conductivity_ec_fuvitech = (send_data_to_rs485(conductivity_command_ec_fuvitech,"EC")) * 1000;
  data_resistivity_ec_fuvitech = send_data_to_rs485(resistivity_command_ec_fuvitech,"EC");
  data_temperateure_ec_fuvitech = send_data_to_rs485(temperateure_command_ec_fuvitech,"EC");
  data_tds_ec_fuvitech = send_data_to_rs485(tds_command_ec_fuvitech,"EC");
  data_salinity_ec_fuvitech = (send_data_to_rs485(tds_command_ec_fuvitech,"EC")/640);
#endif
#if do_fuvitech
  data_dissolved_oxygen_fuvitech = send_data_to_rs485(_command_do_fuvitech,"DO")/100;
#endif
#if SM7820_RS485
  data_air_temperature = send_data_to_rs485(command_sm7820b,"SM7820");
  data_air_temperature= (rx_buffer_sensor[3]<<8|rx_buffer_sensor[4])/100;
  printf("data_air_temperature %.2f\r\n",data_air_temperature);
  data_air_humidity= (rx_buffer_sensor[5]<<8|rx_buffer_sensor[6])/100;
  printf("data_air_humidity %.2f\r\n",data_air_humidity);
  HAL_Delay(1000);
#endif
#if LX200K_485
  data_light_lx200k = send_data_to_rs485(command_lx200k_485,"LK200K");
  data_light_lx200k=(rx_buffer_sensor[3]<<24|rx_buffer_sensor[4]<<16|rx_buffer_sensor[5]<<8|rx_buffer_sensor[6])/1000;
  printf("data_light_sensor %.2f\r\n",data_light_lx200k);
  HAL_Delay(1000);
#endif

#if RK500_22
  data_ph_rk500_22 = send_data_to_rs485(command_rk500_22,"RK500_22");
  data_ph_rk500_22=modbus_bytes_to_float(rx_buffer_sensor, 3);
  printf("data_light_sensor %.2f\r\n",data_ph_rk500_22);
  HAL_Delay(1000);
#endif

#if RK520_01
	data_soil_temperature_rika520_01 = send_data_to_rs485(command_rika520_01,"RK520_01");

	data_soil_temperature_rika520_01 = (rx_buffer_sensor[3]<<8 | rx_buffer_sensor[4])*0.1;
	printf("data_temperature_rika502_01 %.2f\r\n",data_soil_temperature_rika520_01);

	data_soil_moisture_rika520_01 = ((rx_buffer_sensor[5]<<8 | rx_buffer_sensor[6])*0.1)*1.35;
	printf("data_moisture_rika502_01 %.2f\r\n",data_soil_moisture_rika520_01);
	HAL_Delay(1000);
#endif


#if RK520_02
	data_soil_temperature_rika502_02 = send_data_to_rs485(command_rk500_22,"RK520_02");

	data_soil_temperature_rika502_02 = (rx_buffer_sensor[3]<<8 | rx_buffer_sensor[4]);
	printf("data_temperature_rika502_01 %.2f\r\n",data_soil_temperature_rika502_02);

	data_soil_moisture_rika502_02 = (rx_buffer_sensor[5]<<8 | rx_buffer_sensor[6]);
	printf("data_moisture_rika502_01 %.2f\r\n",data_soil_moisture_rika502_02);
	HAL_Delay(1000);
  HAL_Delay(1000);
#endif

#if NBL_S_TMC_7
  data_soil_temperature_niubol_7 = send_data_to_rs485(command_niubol_7in1,"NIUBOL 7IN1");

  data_soil_temperature_niubol_7=(rx_buffer_sensor[3]<<8 | rx_buffer_sensor[4])*0.1;
  printf("data_soil_temperature_niubol_7 %.2f\r\n",data_soil_temperature_niubol_7);

  data_soil_moisture_niubol_7 = (rx_buffer_sensor[5]<<8 | rx_buffer_sensor[6])*0.1;
  printf("data_soil_moisture_niubol_7 %.2f\r\n",data_soil_moisture_niubol_7);

  data_soil_conductivity_niubol_7=(rx_buffer_sensor[7]<<8 | rx_buffer_sensor[8]);
  printf("data_soil_conductivity_niubol_7 %.2f\r\n",data_soil_conductivity_niubol_7);

  data_soil_pH_niubol_7=(rx_buffer_sensor[9]<<8 | rx_buffer_sensor[10])*0.01;
  float temp_pH_niubol_7;
  temp_pH_niubol_7 = data_soil_pH_niubol_7;
  if (6.51 < temp_pH_niubol_7 && temp_pH_niubol_7 < 7)
  {
      data_soil_pH_niubol_7 = data_soil_pH_niubol_7 - 0.8;
  }
  else if (7.01 < temp_pH_niubol_7 && temp_pH_niubol_7 < 8)
  {
      data_soil_pH_niubol_7 = data_soil_pH_niubol_7 - 1.5;
  }
  else if (8.01 < temp_pH_niubol_7 && temp_pH_niubol_7 < 8.5)
  {
      data_soil_pH_niubol_7 = data_soil_pH_niubol_7 - 2;
  }
  printf("data_soil_pH_niubol_7 %.2f\r\n",data_soil_pH_niubol_7);

  data_soil_nitrogen_niubol_7=(rx_buffer_sensor[11]<<8 | rx_buffer_sensor[12]);
  printf("data_soil_nitrogen_niubol_7 %.2f\r\n",data_soil_nitrogen_niubol_7);

  data_soil_phospho_niubol_7=(rx_buffer_sensor[13]<<8 | rx_buffer_sensor[14]);
  printf("data_soil_phospho_niubol_7 %.2f\r\n",data_soil_phospho_niubol_7);

  data_soil_kali_niubol_7=(rx_buffer_sensor[15]<<8 | rx_buffer_sensor[16]);
  printf("data_soil_kali_niubol_7 %.2f\r\n",data_soil_kali_niubol_7);
  HAL_Delay(1000);
#endif

#if NBL_S_NPK
  data_soil_nitrogen_NBL_S_NPK = send_data_to_rs485(command_nbl_s_npk,"NIUBOL_S_NPK");

  data_soil_nitrogen_NBL_S_NPK=(rx_buffer_sensor[3]<<8 | rx_buffer_sensor[4]);
  printf("data_soil_temperature_niubol_7 %.2f\r\n",data_soil_nitrogen_NBL_S_NPK);

  data_soil_phospho__NBL_S_NPK = (rx_buffer_sensor[5]<<8 | rx_buffer_sensor[6]);
  printf("data_soil_moisture_niubol_7 %.2f\r\n",data_soil_phospho__NBL_S_NPK);

  data_soil_kali__NBL_S_NPK=(rx_buffer_sensor[7]<<8 | rx_buffer_sensor[8]);
  printf("data_soil_conductivity_niubol_7 %.2f\r\n",data_soil_kali__NBL_S_NPK);


  HAL_Delay(1000);
#endif



}

