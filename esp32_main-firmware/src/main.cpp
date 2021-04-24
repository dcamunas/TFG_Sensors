#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
//#include <SPI.h>
#include <PubSubClient.h>

#include <definitions.h>
#include <iWifi.h>

/* Global variables */
WiFiClient esp_client;
iWifi my_wifi(ssid, password, esp_client);
PubSubClient client(esp_client);


// ------------------------------------------------------------------------------

void setup()
{
    Serial.begin(9600);
    my_wifi.setup_sta_mode();
    
    
    //client.setServer(mqtt_server, 1883);

}

// ------------------------------------------------------------------------------

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    //String clientId = "ESP8266Client";
    //clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


// ------------------------------------------------------------------------------

void loop()
{
    /*if(!client.connected())
    {
        reconnect();
    }

    client.loop();*/
}
