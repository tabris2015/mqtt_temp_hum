#ifndef PARAM_H
#define PARAM_H

#define DHTTYPE DHT11
#define dht_dpin 0

#define PIR_PIN 4
#define STATUS_LED_PIN 14   // d5
#define ERROR_LED_PIN 12    // d6
#define OUT_PIN 5

#define SSID "ROSnet"
#define PASS "ROSNET2018"

// datos del servidor raspi

#define MQTT_SERVER "192.168.0.105"
#define MQTT_USER ""
#define MQTT_PASS ""

#define TEMP_TOPIC "sensor/temp"
#define HUM_TOPIC "sensor/hum"
#define PIR_TOPIC "sensor/mov"
#define OUT_TOPIC "control/ventilador"

#endif // !PARAM_H