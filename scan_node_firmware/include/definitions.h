#include <Arduino.h>

#define BAUD_RATE 115200

/* Network´s values */
#define MOSQUITTO_PORT 1883
const char *SSID = "Home";
const char *PASSWORD = "2+5+26+28=cumples";
const char *MQTT_SERVER_NUC = "192.168.1.141";
const char *MQTT_SERVER_VM = "192.168.1.175";

/* Promiscuous mode */
#define OFFLINE 0
#define ONLINE  1
#define LAST_CHANNEL 13
#define MAX_SEND_TIME 30000

/* CO2 configurations */
#define CO2_PIN 35
#define MAX_CO2_VALUE 2500
#define MIN_CO2_VALUE 100

/* BLE mode */
#define SCAN_TIME 10

/* MQTT Topics */
//#define ACTIVATE    1
//#define DESACTIVATE 0
String NODE_ID = "1";
String scan_topic = "node/" + NODE_ID + "/scan";

