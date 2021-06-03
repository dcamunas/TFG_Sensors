#include <Arduino.h>
#include <vector>

#define BAUD_RATE 115200

/* Network´s values */
#define MOSQUITTO_PORT 1883
const char *SSID = "Home";
const char *PASSWORD = "2+5+26+28=cumples";
const char *MQTT_SERVER_VM = "192.168.1.188";

#define MAX_SEND_TIME 30000

/* PIR values */
#define PIR1_PIN 15
#define PIR2_PIN 4


/* MQTT Topics */
#define BLE_INDEX       0
#define WIFI_INDEX      1
#define CO2_INDEX       2
#define PEOPLE_INDEX    3

String NODE_ID = "1";
String scan_topic = "node/" + NODE_ID + "/scan";
String env_topic = "node/" + NODE_ID + "/room";


#define SEPARATOR ','
#define EMPTY      0

// My Stringtokenizer
std::vector<String> my_stringtokenizer(String data)
{
  std::vector<String> array;
  String data_aux;
  for (unsigned int i = 0; i < data.length(); i++)
  {
    if(data[i] != SEPARATOR)
        data_aux += data[i];
    else
    {
      array.push_back(data_aux);
      data_aux.clear();
    }
  }
  // Last parameter (CO2)
  array.push_back(data_aux);
  return array;
}

/* Line protocol format */
String line_protocol(String ble, String wifi, String co2, String people)
{
  return "room,location=us-midwest ble=" + ble + " wifi=" + wifi + " co2=" + co2 + " people=" + people;
}
