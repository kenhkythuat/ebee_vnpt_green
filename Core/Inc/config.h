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

#define a7672s 1
#define a7670c 2
#define a7670sa 3
#define a7080 4  // NB-IOT
#define a7680 5  // 4G

#ifndef VERSION_EBEE
#define VERSION_EBEE "1.4.0"
#endif

/* OTA endpoint must be reachable by the SIMCOM modem without credentials. */
#ifndef OTA_ENABLE
#define OTA_ENABLE 1
#endif
#ifndef OTA_MANIFEST_URL
#define OTA_MANIFEST_URL "https://raw.githubusercontent.com/kenhkythuat/ebee_vnpt_green/refs/heads/feature/connect_server_vnpt/ota/manifest.json"
#endif
#ifndef OTA_DEVICE_ID
#define OTA_DEVICE_ID "ebee-stm32f103ret6"
#endif

/* Optional per-machine configuration. This file is ignored by Git. */
#if defined(__has_include)
#if __has_include("config_local.h")
#include "config_local.h"
#endif
#endif

// Codename of the farm, where we deploy this node to.
#ifndef FARM
#define FARM "gateway-agriconnect"
#endif

#ifndef SIMCOM_MODEL
#define SIMCOM_MODEL a7680
#endif

#ifndef INTERVAL_PUPLISH_DATA
#define INTERVAL_PUPLISH_DATA 10 // the time the device sends data to the server
#endif

/* Factory values are copied once by the bootloader to 0x08007800. Legacy
 * config_local.h names remain accepted so existing factory files still work. */
#ifndef FACTORY_SERIAL_NUMBER
#ifdef SERIAL_NUMBER
#define FACTORY_SERIAL_NUMBER SERIAL_NUMBER
#else
#define FACTORY_SERIAL_NUMBER "eb_000002"
#endif
#endif

#ifndef FACTORY_MQTT_USER
#ifdef MQTT_USER
#define FACTORY_MQTT_USER MQTT_USER
#else
#define FACTORY_MQTT_USER ""
#endif
#endif

#ifndef FACTORY_MQTT_PASS
#ifdef MQTT_PASS
#define FACTORY_MQTT_PASS MQTT_PASS
#else
#define FACTORY_MQTT_PASS ""
#endif
#endif

#define MQTT_TOPIC_SENSOR_THINGSBOARD "v1/devices/me/telemetry"
#define MQTT_TOPIC_ATTRIBUTES_THINGSBOARD "v1/devices/me/attributes"


// MQTT topic to subscribe and get command to switch on/off actuator
//#define MQTT_TOPIC_ACTUATOR_CONTROL FARM "/snac/" SERIAL_NUMBER "/"
/** Default VNPT demo MQTT broker. */
#ifndef MQTT_HOST
#define MQTT_HOST "tcp://iot-demo.vnptgreen.vn" // MQTT broker
#endif

#ifndef FACTORY_MQTT_CLIENT_ID
#ifdef MQTT_CLIENT_ID
#define FACTORY_MQTT_CLIENT_ID MQTT_CLIENT_ID
#else
#define FACTORY_MQTT_CLIENT_ID "dttxzh7awplus0q2xmbg"
#endif
#endif

#ifndef MQTT_PORT
#define MQTT_PORT 1883
#endif

#ifndef RK520_01
#define RK520_01 false
#endif
#ifndef RK520_02
#define RK520_02 false
#endif
#ifndef RK500_02
#define RK500_02 false
#endif
#ifndef RK500_22
#define RK500_22 false
#endif
#ifndef bh1750
#define bh1750 false
#endif
#ifndef sht30
#define sht30 false
#endif
#ifndef NBL_S_TMC_7
#define NBL_S_TMC_7 true
#endif
#ifndef NBL_S_NPK
#define NBL_S_NPK false
#endif
#ifndef CO2_UART
#define CO2_UART false
#endif
#ifndef SM7820_RS485
#define SM7820_RS485 true
#endif
#ifndef LX200K_485
#define LX200K_485 true
#endif
#ifndef PT100_01
#define PT100_01 false
#endif
#ifndef PT100_02
#define PT100_02 false
#endif
#ifndef YW001
#define YW001 false
#endif
#ifndef enable_sleep_mode
#define enable_sleep_mode false
#endif


#define TIME_PERIOD ((4000000*INTERVAL_PUPLISH_DATA)/60000)-1;

#endif /* INC_CONFIG_H_ */
