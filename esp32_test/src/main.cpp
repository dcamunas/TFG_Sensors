#include <Arduino.h>
#include <PubSubClient.h>
#include <vector>
#include <sstream>
#include <string>
#include <WiFi.h>
#include <iWifi.h>
#include <definitions.h>


WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);
std::vector<std::string> mqtt_data;


void string_tokenizer(std::vector<std::string> mqtt_data, std::string line)
{  
    // Vector of string to save tokens 
    std::vector <std::string> words_vector; 
    std::stringstream check(line);  
    std::string word; 
      
    while(getline(check, word, ',')) 
        mqtt_data.push_back(word); 
}

void reconnect();

void callback(char *topic, byte *payload, size_t length)
{
  std::string s_payload(reinterpret_cast <const char *>(payload), length);
  string_tokenizer(s_payload, mqtt_data);
}

void setup()
{
  Serial.begin(BAUD_RATE);

  my_wifi.connect();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  client.setCallback(callback);
  reconnect();
  client.subscribe(scan_topic.c_str());
}

void loop()
{
  client.loop();
  for (size_t i = 0; i < mqtt_data.size(); i++)
  {
    Serial.print(String(mqtt_data[i].c_str()) + " | ");
  }  
  Serial.println();
  delay(2000);

  //client.disconnect();
}

void reconnect()
{
  while (!client.connected())
  {
    if (!client.connect("esp32_test"))
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(2500);
    }
    yield();
  }
}
