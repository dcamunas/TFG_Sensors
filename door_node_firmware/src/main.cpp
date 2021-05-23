#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <iWifi.h>
#include <definitions.h>

WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);

int people_counter, old_people_counter;
unsigned int s1_value, s2_value;
boolean s1_detected, s2_detected;
unsigned long send_time;
std::vector<String> data;
String mqtt_recv_data;

void reconnect();
void counter_people();
void send_data();
void callback(char *topic, byte *payload, unsigned int length);
boolean check_data();
boolean check_send_time();


void setup() 
{
  people_counter = old_people_counter = 0;
  s1_detected = s2_detected = false;
  s1_value = s2_value = 0;
  send_time = 0;

  Serial.begin(BAUD_RATE);
  pinMode(PIR1_PIN, INPUT_PULLDOWN);
  pinMode(PIR2_PIN, INPUT_PULLDOWN);

  my_wifi.connect();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  client.setCallback(callback);
  reconnect();
  client.subscribe(scan_topic.c_str());
}

void show_list(std::vector<String> list)
{
  Serial.print("[ ");
  for (size_t i = 0; i < list.size(); i++)
  {
    Serial.print(list[i].c_str());
    Serial.print(", ");
  }
  Serial.println("]");
  
}

void loop() 
{ 
  /*if(!client.connected())
    reconnect();*/
  client.loop();
  
  Serial.println("L: " + mqtt_recv_data);

  s2_value = digitalRead(PIR2_PIN);
  //delay(250);
  s1_value = digitalRead(PIR1_PIN);

  counter_people();

  if(check_data() || check_send_time())
    send_data();

  delay(5000);
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

void callback(char *topic, byte *payload, unsigned int length)
{
  mqtt_recv_data.clear();
  for (unsigned int i = 0; i < length; i++)
    if(char(payload[i]) != '[' && char(payload[i]) != ']')
      mqtt_recv_data += char(payload[i]);
  
  Serial.println(mqtt_recv_data);
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

}

boolean check_data()
{
  boolean send = false;
  if(data.size() == EMPTY)
    send += true;

  if(people_counter != old_people_counter)
  {
    old_people_counter = people_counter;
    send += true;
  }

  return send;
}

boolean check_send_time()
{
  return (millis() - send_time) >= MAX_SEND_TIME;
}

void send_data()
{
  data.push_back(String(people_counter));

  client.loop();
  //client.publish(env_topic.c_str(), line_protocol(data[BLE_INDEX], data[WIFI_INDEX], data[CO2_INDEX], data[PEOPLE_INDEX]).c_str());
  //client.disconnect();

  send_time = millis();

  //data.clear();

}
