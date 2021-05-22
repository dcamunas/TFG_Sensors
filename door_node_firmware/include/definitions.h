#include <Arduino.h>
#include <vector>

#define BAUD_RATE 9600

/* Network´s values */
#define MOSQUITTO_PORT 1883
const char *SSID = "Home";
const char *PASSWORD = "2+5+26+28=cumples";
const char *MQTT_SERVER_NUC = "192.168.1.141";
const char *MQTT_SERVER_VM = "192.168.1.175";

#define MAX_SEND_TIME 120000

/* PIR values */
#define PIR1_PIN 15
#define PIR2_PIN 4


/* MQTT Topics */
#define BLE_INDEX       0
#define WIFI_INDEX      1
#define CO2_INDEX       2
#define PEOPLE_INDEX    4

String NODE_ID = "1";
String scan_topic = "node/" + NODE_ID + "/scan";
String env_topic = "node/" + NODE_ID + "/environment";


#define SEPARATOR ','
#define EMPTY      0



/* Line protocol format */
String line_protocol(String ble_count, String wifi_count, String co2_level, String people_count)
{
  return "environment,location=us-midwest ble_count=" + ble_count + " wifi_count=" + wifi_count + " co2_level=" + co2_level + " people_count=" + people_count;
}