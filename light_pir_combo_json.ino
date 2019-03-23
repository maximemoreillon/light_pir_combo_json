/*
 * Light and PIR combo
 * Maxime MOREILLON
 * Board type: Wemos D1 Mini
 */

// Libraries
#include <ESP8266WiFi.h> // Main ESP8266 library
#include <ArduinoOTA.h> // OTA update library
#include <WiFiUdp.h> // Required for OTA
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Credentials and device specific configuration
#include "credentials.h";
//#include "light_pir_bathroom_nagoya.h";
//#include "light_pir_corridor_nagoya.h";
//#include "light_pir_entrance_nagoya.h";
#include "light_pir_toilet_nagoya.h";

// MQTT
#define MQTT_BROKER_ADDRESS IPAddress(192, 168, 1, 2)
#define MQTT_PORT 1883
#define MQTT_LAST_WILL "{'state':'disconnected'}"
#define MQTT_QOS 1
#define MQTT_RETAIN true

// Pin mapping
#define  LIGHT_PIN D6
#define  PIR_PIN D2

// Global variables
WiFiClient wifi_client;
PubSubClient MQTT_client(wifi_client);

// Misc variables
char* light_state = INITIAL_STATE;


void setup() {
  // Mandatory initial delay
  delay(10);

  // Serial init
  Serial.begin(115200);
  Serial.println();
  Serial.println(); // Separate serial stream from initial gibberish

  IO_setup();
  wifi_setup();
  MQTT_setup();
  OTA_setup();
}


void loop() {
  ArduinoOTA.handle();
  MQTT_client.loop();
  read_PIR();
  wifi_connection_manager();
  MQTT_connection_manager();
}
