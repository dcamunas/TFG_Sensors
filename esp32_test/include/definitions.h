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
#define PIR1_PIN 15
#define PIR2_PIN 4


/* MQTT Topics */
String NODE_ID = "1";
String scan_topic = "node/" + NODE_ID + "/scan";



String line_protocol_counter(unsigned int counter)
{
    return String(counter);
}