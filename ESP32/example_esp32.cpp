/*   Arduino Sketch to Program an ESP32 to read LSM9DS0 Sensor from
 *   its I2C Bus, convert the data into InfluxDB's Line Protocol Strings
 *   and Publish it to an MQTT broker running on an IoT Edge Device (Pi4)
 *   Author: Shan Desai <https://github.com/shantanoo-desai>
*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>

#include <WiFi.h>
#include <PubSubClient.h>


//WifiClient and MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

// USE I2C bus for ESP32
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0();

/**
 * >>>>>> Adapt Settings here <<<<<<<
 */

// Name of the IoT Node or a unique ID
String SENSOR_ID = "sensor101";

/*WLAN Configuration Settings*/
const char* ssid = "<YOUR_SSID_NAME>";
const char* password = "<YOUR_PASSKEY_HERE>";

/*MQTT Broker Configuration Settings*/
const char* mqtt_broker_address = "<IP_ADDRESS_OF_PI_IN_NETWORK>";
const uint16_t mqtt_port = 1883;
const char* client_id = "IOTNODE_ESP32";

/* MQTT TOPICS */
/*
* Topic structure: IOT/<SENSORID>/acc, IOT/<SENSORID>/mag ...
*/
String acc_topic = "IOT/" + SENSOR_ID + "/acc";
String mag_topic = "IOT/" + SENSOR_ID + "/mag";
String gyro_topic = "IOT/" + SENSOR_ID + "/gyro";
String temp_topic = "IOT/" + SENSOR_ID + "/temp";


/*Reconnect to MQTT Broker*/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("MQTT:reconnect: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(client_id)) {
      Serial.println("MQTT:reconnect: Connected");
    } else {
      Serial.print("MQTT:reconnect: Failed, rc=");
      Serial.print(client.state());
      Serial.println("MQTT:reconnect: Trying Again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

void setupSensor() {
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("LSM MQTT Demo");
  if (!lsm.begin()) {
    Serial.println("Sensor Not Initialized");
    while(1);
  }
  Serial.println("Found Sensor, Setting up");
  setupSensor();
  delay(1000);
  Serial.println("Connecting to WLAN Access-Point");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
 }
 Serial.println("main:setup: WiFi Connected");
 Serial.println("main:setup: IP Address: ");
 Serial.println(WiFi.localIP());
 Serial.println("main:setup: Setting up MQTT Configuration");
 client.setServer(mqtt_broker_address, mqtt_port);
}


/*
* Function to convert temperature value into line protocol
*/ 
String lineprotocol_temp(int temp_value) {

  String _result = "environment temp=" + String(temp_value);
  return _result;
}


/*
* Function to convert acceleration value into line protocol
*/ 
String lineprotocol_acc(float acc_x, float acc_y, float acc_z) {

  String _result = "acceleration accX=" + String(acc_x) + ",accY=" + String(acc_y) + ",accZ=" + String(acc_z);
  return _result;
}


/*
* Function to convert magnetometer value into line protocol
*/ 
String lineprotocol_mag(float mag_x, float mag_y, float mag_z) {

  String _result = "magnetometer magX=" + String(mag_x) + ",magY=" + String(mag_y) + ",magZ=" + String(mag_z);
  return _result;
}



/*
* Function to convert gyroscope value into line protocol
*/ 
String lineprotocol_gyro(float gyro_x, float gyro_y, float gyro_z) {

  String _result = "gyroscope gyroX=" + String(gyro_x) + ",gyroY=" + String(gyro_y) + ",gyroZ=" + String(gyro_z);
  return _result;
}


void loop() {
  if (!client.connected()){
    reconnect();
  }
  client.loop();
  
   /* Get a new sensor event */ 
  sensors_event_t accel, mag, gyro, temp;

  lsm.getEvent(&accel, &mag, &gyro, &temp); 

  client.publish(acc_topic.c_str() , lineprotocol_acc(accel.acceleration.x, accel.acceleration.y, accel.acceleration.z).c_str());
  client.publish(mag_topic.c_str(), lineprotocol_mag(mag.magnetic.x, mag.magnetic.y, mag.magnetic.z).c_str());
  client.publish(gyro_topic.c_str(), lineprotocol_gyro(gyro.gyro.x, gyro.gyro.y, gyro.gyro.z).c_str());
  client.publish(temp_topic.c_str(), lineprotocol_temp(temp.temperature).c_str());
  delay(1000);
}