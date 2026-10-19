/*
 * common_simcom.h
 *
 *  Created on: Sep 15, 2025
 *      Author: MY PC
 */

#include "stdbool.h"

#ifndef INC_COMMON_SIMCOM_H_
#define INC_COMMON_SIMCOM_H_

#include "common_modbus_rs485.h"

void enable_simcom(void);
extern char rx_buffer_simcom[700];
extern char rx_data_sim[700];
extern char rx_uart2[10];
enum GmsModemState
{
	Off,
	On,
	InternetReady,
	MqttReady,
	Subscribed,
	UpdateToServer,
	DisconnectMqtt,
	SleepStm32
};
extern enum GmsModemState current_status_simcom;
extern volatile bool to_send_status_to_server;
extern volatile bool ota_non_modem_io_paused;
//extern current_status_simcom;
void send_to_simcom_a76xx(char *cmd);
bool enable_mqtt_on_gsm_modem(void);
bool acquire_gsm_mqtt_client(void);
bool connect_mqtt_server_by_gsm(void);
bool subscribe_mqtt_via_gsm(void);
bool publish_ota_ack_via_gsm(void);
bool stop_mqtt_via_gsm(void);
void check_handle_state(enum GmsModemState status);
#endif /* INC_COMMON_SIMCOM_H_ */
