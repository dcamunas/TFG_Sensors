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
  Serial.println("VAL: " + digit_sensor_value);
  //Serial.println("ANALOG VALUE: " + analogRead(ANALOG_PIR_PIN));

  delay(2500);
}