#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <iWifi.h>
#include <definitions.h>


WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);

void setup() 
{
  Serial.begin(BAUD_RATE);

  //my_wifi.connect();
  //client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  pinMode(DIGIT_PIR_PIN, INPUT);
  //pinMode(ANALOG_PIR_PIN, INPUT);
}

void loop() 
{
  int digit_sensor_value = digitalRead(DIGIT_PIR_PIN);
  if( digit_sensor_value == HIGH)
    Serial.println("HIGHT");
  else if (digit_sensor_value == LOW)
    Serial.println("LOW");
    
  //Serial.println(analogRead(ANALOG_PIR_PIN));

  delay(1000);
}