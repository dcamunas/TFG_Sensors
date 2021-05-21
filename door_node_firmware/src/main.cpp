#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <iWifi.h>
#include <definitions.h>

WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);
std::vector<std::string> mqtt_data;


int people_counter, old_people_counter;
unsigned int s1_value, s2_value;
boolean s1_detected, s2_detected;

void reconnect();
void counter_people();


void setup() 
{
  people_counter = old_people_counter = 0;
  s1_detected = s2_detected = false;
  s1_value = s2_value = 0;

  Serial.begin(BAUD_RATE);
  pinMode(PIR1_PIN,INPUT_PULLDOWN);
  pinMode(PIR2_PIN, INPUT_PULLDOWN);

  my_wifi.connect();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  client.setCallback(callback);
  if (!client.connected())
    reconnect();
  client.subscribe(scan_topic.c_str());
}

void send_mqtt_data()
{
  client.loop();
  client.publish(env_topic.c_str(), line_protocol(mqtt_data[BLE_INDEX], mqtt_data[WIFI_INDEX], mqtt_data[CO2_INDEX], mqtt_data[PEOPLE_INDEX]).c_str());
  client.disconnect();
}

boolean more_people()
{
  if(people_counter != old_people_counter)
  {
    old_people_counter = people_counter;
    return true;
  }
  return false;
}

void loop() 
{  
  s2_value = digitalRead(PIR2_PIN);
  //delay(250);
  s1_value = digitalRead(PIR1_PIN);

  counter_people();
  if(more_people())
    send_mqtt_data();

  delay(250);
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
  string_tokenizer(mqtt_data, s_payload);
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
}
