#include <Arduino.h>
#include <PubSubClient.h>
#include <definitions.h>
#include<iWifi.h>
#include <iBle.h>


/* Global variables */
WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);
iBle my_ble;

unsigned long send_time;

void reconnect();
void send_mqtt_data();


void setup()
{
  Serial.begin(BAUD_RATE);
  my_wifi.connect_wifi();
  my_ble.setup_ble();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
}

void loop()
{
  boolean send_mqtt = true;

  my_ble.scan_ble(send_mqtt);

  if (send_mqtt)
  {
    send_mqtt_data();
  }
  //delay(SCAN_TIME * 1000);
}

// ------------------------------------------------------------------------------

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    yield();
  }
}

void send_mqtt_data()
{
  String msg;
  /* Setup MQTT */
  if (!client.connected())
    reconnect();
  client.loop();

  msg = String(my_ble.get_devices_number());
  if (client.publish("test", msg.c_str()))
    Serial.println("Succesfully published.\n");
  else
    Serial.println("\nMessage not published\n");

  client.disconnect();

  send_time = millis();
}
