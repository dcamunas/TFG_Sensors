#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <string>
#include <iWifi.h>
#include <definitions.h>

WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);
std::vector<std::string> current_data, old_data;


int people_counter;
unsigned int s1_value, s2_value;
boolean s1_detected, s2_detected;


void reconnect();
void counter_people();
void send_mqtt_data();
void callback(char *topic, byte *payload, size_t length);
boolean check_data();


void setup() 
{
  people_counter = 0;
  s1_detected = s2_detected = false;
  s1_value = s2_value = 0;

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

void loop() 
{  
  s2_value = digitalRead(PIR2_PIN);
  //delay(250);
  s1_value = digitalRead(PIR1_PIN);

  counter_people();

  if(check_data())
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
    
  delay(250);
  
  current_data.push_back(std::to_string(people_counter));
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

void send_mqtt_data()
{
  client.loop();
  client.publish(env_topic.c_str(), line_protocol(current_data[BLE_INDEX], current_data[WIFI_INDEX], current_data[CO2_INDEX], current_data[PEOPLE_INDEX]).c_str());
  client.disconnect();
  current_data.clear();
}