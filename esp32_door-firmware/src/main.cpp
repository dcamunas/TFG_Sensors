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
boolean s1_detected = false, s2_detected = false;

String msg;

void reconnect();

void setup() 
{

  Serial.begin(BAUD_RATE);
  pinMode(PIR2_PIN,INPUT_PULLDOWN);
  pinMode(PIR1_PIN, INPUT_PULLDOWN);

  my_wifi.connect();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
}

void counter_people()
{
  switch (s1_value)
  {
  case HIGH:
    if(!s1_detected)
      s1_detected = true;
      
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

  if(s1_detected == true && s2_detected == true)
  {
    delay(1000);
    s1_detected = s2_detected = false;
  }
}


void loop() 
{  
  people_counter = 0;

  s1_value = digitalRead(PIR1_PIN);
  delay(250);
  s2_value = digitalRead(PIR2_PIN);
  counter_people();

 if (!client.connected())
    reconnect();
  client.loop();
  
  msg = "s1-" + String(s1_value) + "||s2-" + String(s2_value) + " || C:" + String(people_counter);
  client.publish("test", msg.c_str());

  client.disconnect();

  delay(500);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection... ");
    if (client.connect("esp32_main-client1"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(2500);
    }
    yield();
  }
}

//esp_sleep_enable_ext1_wakeup(ESP_EXT1_WAKEUP_ALL_LOW);

/*void IRAM_ATTR notify_main()
{
  client.publish()
}*/