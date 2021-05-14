#include <Arduino.h>

#define BAUD_RATE 9600

/* Network´s values */
#define MOSQUITTO_PORT 1883
const char *SSID = "Home";
const char *PASSWORD = "2+5+26+28=cumples";
const char *MQTT_SERVER_NUC = "192.168.1.141";
const char *MQTT_SERVER_VM = "192.168.1.175";

#define MAX_SEND_TIME 300000

/* PIR values */
#define PIR1_PIN 4
#define PIR2_PIN 15


/* MQTT Topics */
#define ACTIVATE    1
#define DESACTIVATE 0
String NODE_ID = "1";
String env_topic = "NODE/" + NODE_ID + "/activation";
