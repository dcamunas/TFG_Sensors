#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <iostream>
#include <iWifi.h>
#include <definitions.h>

WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);
std::vector<std::string> current_data, old_data;


int people_counter;
unsigned int s1_value, s2_value;
boolean s1_detected, s2_detected;
unsigned long send_time;


void reconnect();
void counter_people();
void send_mqtt_data();
void callback(char *topic, byte *payload, size_t length);
boolean check_data();
std::string to_string(int number);
boolean check_send_time();


void setup() 
{
  people_counter = 0;
  s1_detected = s2_detected = false;
  s1_value = s2_value = 0;
  send_time = 0;

  Serial.begin(BAUD_RATE);
  pinMode(PIR1_PIN, INPUT_PULLDOWN);
  pinMode(PIR2_PIN, INPUT_PULLDOWN);

  my_wifi.connect();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  client.setCallback(callback);
  if (!client.connected())
    reconnect();
  client.subscribe(scan_topic.c_str());
}

void show_list(std::vector<std::string> list)
{
  Serial.println("[");
  for (size_t i = 0; i < list.size(); i++)
  {
    Serial.print(list[i].c_str());
    Serial.print(", ");
  }
  Serial.println("]");
  
}

void loop() 
{  
  s2_value = digitalRead(PIR2_PIN);
  //delay(250);
  s1_value = digitalRead(PIR1_PIN);

  counter_people();

  if(check_data() || check_send_time())
    send_mqtt_data();

}

void reconnect()
{
  while (!client.connected())
  {
    if (!client.connect("esp32_door"))
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(2500);    
    }
    yield();
  }
}

void callback(char *topic, byte *payload, size_t length)
{
  std::string s_payload(reinterpret_cast <const char *>(payload), length);
  string_tokenizer(current_data, s_payload);
}

void counter_people()
{
  switch (s1_value)
  {
  case HIGH:
    if(!s1_detected)
      s1_detected = true;
    else
      if(s2_detected)
        people_counter > 0 ? people_counter-- : people_counter = 0;
    break;
  // LOW
  default:
    s1_detected = false;
    break;
  }

  switch (s2_value)
  {
  case HIGH:
    if(!s2_detected)
      s2_detected = true;
    else
      if(s1_detected)
        people_counter++;
    break;  
  // LOW
  default:
    s2_detected = false;
    break;
  }

  if(s1_detected && s2_detected)
    s1_detected = s2_detected = false;

  current_data.push_back(to_string(people_counter));    
  delay(250);

}

boolean check_data()
{
  boolean send = false;

  for(unsigned int i = 0; i < current_data.size(); i++)
  {
    if(current_data[i] != old_data[i])
      old_data[i] = current_data[i];
      send += true; 
  }

  return send;
}

boolean check_send_time()
{
  return (millis() - send_time) >= MAX_SEND_TIME;
}

void send_mqtt_data()
{
  client.loop();
  client.publish(env_topic.c_str(), line_protocol(current_data[BLE_INDEX], current_data[WIFI_INDEX], current_data[CO2_INDEX], current_data[PEOPLE_INDEX]).c_str());
  client.disconnect();
  
  send_time = millis();
  current_data.clear();
}

std::string to_string(int number)
{
  std::stringstream aux;
  std::string result;
  aux << number;
  aux >> result;

  return result;
}