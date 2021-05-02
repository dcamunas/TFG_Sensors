#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
//#include <SPI.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char *ssid = "Home";
const char *password = "David1208Helena";
const char *mqtt_server = "192.168.1.170";

const unsigned int co2_pin = 35;


WiFiClient esp_client;
PubSubClient client(esp_client);

// Create variable to hold mqtt messages
#define MSG_BUFFER_SIZE (100)
char msg[MSG_BUFFER_SIZE];

void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to " + String(ssid));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(50);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("\nWiFi connected");
  Serial.println("IP address: " + String(WiFi.localIP()));
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

void setup()
{
  Serial.begin(9600);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  pinMode(co2_pin, INPUT);
  
}

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

void loop()
{
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  String message = String("concentration,location=us ppm="+String(read_co2_sensor()));
  message.toCharArray(msg, message.length());
  Serial.println(msg);

  // Send the message on the sensors topic
  client.publish("test", msg);

  delay(5000);

}