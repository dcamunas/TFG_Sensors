#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
//#include <SPI.h>
#include <PubSubClient.h>

#include <definitions.h>

/* Global variables */
WiFiClient esp_client;
PubSubClient client(esp_client);


void setup_wifi()
{
    delay(10);
    Serial.println("Connecting to " + String(ssid));

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    //randomSeed(micros());

    Serial.println("\nWiFi connected");
    Serial.println("IP address: " + String(WiFi.localIP()));
}

// ------------------------------------------------------------------------------

void setup()
{
    Serial.begin(9600);

    setup_wifi();
    client.setServer(mqtt_server, 1883);

    pinMode(CO2_PIN, INPUT);
}

// ------------------------------------------------------------------------------

float read_co2_sensor(){
  int sensor_value = analogRead(co2_pin), voltage_diference;
  Serial.println(sensor_value);

  // The analog signal is converted to a voltage
  float concentration = 0.0, voltage = sensor_value * (3300 / 4095.0);

  if (voltage == 0)
  {
    Serial.println("Fault");
  }
  else if (voltage < 400)
  {
    Serial.println("preheating");
  }
  else
  {
    voltage_diference = voltage - 400;
    concentration = (voltage_diference * 50.0) / 16.0;
    
    // Print Voltage and concentration
    //Serial.println("[X] Voltage: " + String(voltage) + " mv  ||  Concentration: " + String(concentration) + " ppm.");
  }

  return concentration;
}

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
    if(!client.connected())
    {
        reconnect();
    }

    client.loop();

  String message = String("concentration,location=us ppm="+String(read_co2_sensor()));

  Serial.println(message);
  client.publish("sensors", message.c_str());

  delay(5000);
}
