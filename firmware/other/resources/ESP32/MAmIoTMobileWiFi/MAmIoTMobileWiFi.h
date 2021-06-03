#include <WiFi.h>

// ================================================================
// ===                      Wi-Fi consts                        ===
// ================================================================
const char* SSID_STATION = "MAmI";
const char* PASSWORD_STATION = "Qazwsxedc1234";

// ================================================================
// ===                      MQTT variables                      ===
// ================================================================
// Header for MQTT PubSubClient
#include "PubSubClient.h"
const char* MQTT_SERVER = "192.168.43.96";
const unsigned int MQTT_PORT = 1883;
const unsigned int QOS_SUBSCRIBE = 1; // PubSubClient accepts QoS 0/1 when receiving MQTT messages

//////////////////////////MQTT variables///////////////////////////
const char* MESSAGE_MOBILE_TOPIC = "mobiles/messages";  // Topic to to publish the button switch

WiFiClient espClient;
PubSubClient client(espClient);
