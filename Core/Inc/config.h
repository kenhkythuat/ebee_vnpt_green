/*
 * config.h
 *
 *  Created on: AUG 1, 2024
 *      Author: thuanphat7
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "stdio.h"
#include <string.h>

// Codename of the farm, where we deploy this node to.

//#define FARM "minhhuyagri"
#define FARM "queen-ecosmart-farm"
#define a7672s 1
#define a7670c 2
#define a7670sa 3
#define a7080 4  // NB-IOT
#define a7680 5  // 4G

#define SIMCOM_MODEL a7680 // #default is a7670c if you use model other please choose enter your
         // model
#define INTERVAL_PUPLISH_DATA 180 // the time the device sends data to the server
// Serial number. Must be lower case.
#ifndef SERIAL_NUMBER
#define SERIAL_NUMBER "eb000384"
#endif

//#define MQTT_USER "node"   // User - connect to MQTT broker
//#define MQTT_PASS "654321" // Password - connect to MQTT broker

#define MQTT_USER "mqttnode"       // User - connect to MQTT broker
#define MQTT_PASS "congamo"		// Password - connect to MQTT broker

#define MQTT_TOPIC_ACTUATOR_STATUS FARM "/sn/" SERIAL_NUMBER
// MQTT topic to subscribe and get command to switch on/off actuator
//#define MQTT_TOPIC_ACTUATOR_CONTROL FARM "/snac/" SERIAL_NUMBER "/"
/** MQTT
 * Global broker: mqtt.agriconnect.vn`
 */
#define MQTT_HOST "tcp://mqtt.agriconnect.vn" // MQTT broker

#define MQTT_CLIENT_ID SERIAL_NUMBER
#define MQTT_PORT 1883

#define RK520_01 true
#define RK520_02 false
#define RK500_02 false
#define RK500_22 true
#define bh1750 false
#define sht30 false
#define NBL_S_TMC_7 false
#define NBL_S_NPK true
#define co2 false
#define SM7820_RS485 true
#define LX200K_485 true
#define enable_sleep_mode false


#define TIME_PERIOD ((4000000*INTERVAL_PUPLISH_DATA)/60000)-1;

#endif /* INC_CONFIG_H_ */
