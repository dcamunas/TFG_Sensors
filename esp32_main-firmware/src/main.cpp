#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
//#include <SPI.h>
#include <PubSubClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertisedDevice.h>
#include <BLEScan.h>

#include <definitions.h>
#include <iWifi.h>
#include <vector>
#include <utils.h>

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
  }
};

/* Global variables */
WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);

unsigned long send_time;
BLEScan *ble_scan;

void setup_ble();
void scan_ble(boolean &send_mqtt);
void reconnect();
void send_mqtt_data();

void setup()
{
  Serial.begin(BAUD_RATE);
  setup_ble();
}

void loop()
{
  boolean send_mqtt = false;

  //scan_ble(send_mqtt);

  if (send_mqtt)
  {
    //my_wifi.show_people();
    send_mqtt_data();
  }
  delay(SCAN_TIME * 1000);
}

// ------------------------------------------------------------------------------


void send_mqtt_data()
{
  String msg;
  /* Setup MQTT */
  my_wifi.connect_wifi();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  if (!client.connected())
    reconnect();
  client.loop();

  msg = String("concentration, location=us, ppm=1");
  if (client.publish("test", msg.c_str()))
    Serial.println("Succesfully published.\n");
  else
    Serial.println("\nMessage not published\n");

  client.disconnect();

  send_time = millis();
}

// ------------------------------------------------------------------------------

void setup_ble()
{
  BLEDevice::init("");
  ble_scan = BLEDevice::getScan(); //create new scan
  //ble_scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  ble_scan->setActiveScan(true); //active scan uses more power, but get results faster
  ble_scan->setInterval(100);
  ble_scan->setWindow(99);  // less or equal setInterval value
}

void scan_ble(boolean &send_mqtt)
{
  BLEScanResults foundDevices = ble_scan->start(SCAN_TIME, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  ble_scan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000);
}
