/*
 * Example per-machine configuration.
 *
 * Copy this file to "config_local.h", then replace the placeholder values:
 *
 *   PowerShell:
 *   Copy-Item Core/Inc/config_local.example.h Core/Inc/config_local.h
 *
 *   Git Bash:
 *   cp Core/Inc/config_local.example.h Core/Inc/config_local.h
 *
 * config_local.h is ignored by Git. Never put production credentials in this
 * example file.
 */
#ifndef INC_CONFIG_LOCAL_H_
#define INC_CONFIG_LOCAL_H_

#define FARM "your-farm"
#define SIMCOM_MODEL a7680
#define INTERVAL_PUPLISH_DATA 10
#define SERIAL_NUMBER "eb_xxxxxx"

#define VERSION_EBEE "1.0.0"
#define OTA_ENABLE 1
#define OTA_MANIFEST_URL "https://gitlab.com/agriconnect/embedded/ebee_stm32f103ret6/-/raw/ota_ebee_v1/ota/manifest.json"
#define OTA_DEVICE_ID "ebee-stm32f103ret6"

#define MQTT_USER "your_mqtt_username"
#define MQTT_PASS "your_mqtt_password"
#define MQTT_HOST "tcp://iot-demo.vnptgreen.vn"
#define MQTT_CLIENT_ID "your_mqtt_client_id"
#define MQTT_PORT 1883

#define RK520_01 false
#define RK520_02 false
#define RK500_02 false
#define RK500_22 false
#define bh1750 false
#define sht30 false
#define NBL_S_TMC_7 false
#define NBL_S_NPK false
#define CO2_UART false
#define SM7820_RS485 false
#define LX200K_485 false
#define PT100_01 false
#define PT100_02 false
#define YW001 false
#define enable_sleep_mode false

#endif /* INC_CONFIG_LOCAL_H_ */
