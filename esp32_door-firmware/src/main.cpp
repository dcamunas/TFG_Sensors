#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <iWifi.h>
#include <definitions.h>


WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);

int s1_value, s2_value;
String msg;

void reconnect();

void setup() 
{
  Serial.begin(BAUD_RATE);
  pinMode(PIR2_PIN,INPUT_PULLUP);
  pinMode(PIR1_PIN, INPUT_PULLUP);

  my_wifi.connect();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  //pinMode(ANALOG_PIR_PIN, INPUT);
}

void loop() 
{
  s1_value = digitalRead(PIR1_PIN);
  delay(150);
  s2_value = digitalRead(PIR2_PIN);

 if (!client.connected())
    reconnect();
  client.loop();
  
  msg = "s1-" + String(s1_value) + "|s2-" + String(s2_value);
  client.publish("test", msg.c_str());

  client.disconnect();

  delay(400);
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