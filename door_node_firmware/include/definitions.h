#include <Arduino.h>
#include <vector>
#include <sstream>
#include <string>

#define BAUD_RATE 115200

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
#define BLE_INDEX       0
#define WIFI_INDEX      1
#define CO2_INDEX       2
#define PEOPLE_INDEX    4

String NODE_ID = "1";
String scan_topic = "node/" + NODE_ID + "/scan";
String env_topic = "node/" + NODE_ID + "/environment";


void string_tokenizer(std::vector<std::string> mqtt_data, std::string line)
{  
    // Vector of string to save tokens 
    std::vector <std::string> words_vector; 
    std::stringstream check(line);  
    std::string word; 
      
    while(getline(check, word, ',')) 
        mqtt_data.push_back(word); 
}

/* Line protocol format */
std::string line_protocol(std::string ble_count, std::string wifi_count, std::string co2_level, std::string people_count)
{
  return "environment,location=us-midwest ble_count=" + ble_count + " wifi_count=" + wifi_count + " co2_level=" + co2_level + " people_count=" + people_count;
}