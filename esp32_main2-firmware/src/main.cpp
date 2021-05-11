#include <Arduino.h>
#include <PubSubClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>


#include <definitions.h>
#include <utils.h>
#include <iWifi.h>


/* Global variables */
WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);
BLEScan *ble_scan;

unsigned long send_time;    
unsigned int devices_number;


void reconnect();
void setup_ble();
void scan_ble(boolean &send_mqtt);
void send_mqtt_data();


void setup()
{
  Serial.begin(BAUD_RATE);
  my_wifi.connect_wifi();
  setup_ble();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
}

void loop()
{
  boolean send_mqtt = true;

  scan_ble(send_mqtt);

  /*if (send_mqtt)
  {
    send_mqtt_data();
  }*/
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

  msg = String(devices_number);
  if (client.publish("test", msg.c_str()))
    Serial.println("Succesfully published.\n");
  else
    Serial.println("\nMessage not published\n");

  client.disconnect();

  send_time = millis();
}

void setup_ble()
{
    BLEDevice::init("");
    ble_scan = BLEDevice::getScan(); //create new scan
    ble_scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    ble_scan->setActiveScan(true); //active scan uses more power, but get results faster
    ble_scan->setInterval(100);
    ble_scan->setWindow(99); // less or equal setInterval value
}

void scan_ble(boolean &send_mqtt)
{
    BLEScanResults found_devices = ble_scan->start(SCAN_TIME, false);
    devices_number = found_devices.getCount();
    Serial.println(devices_number);
    Serial.println("Scan done!");
    ble_scan->setActiveScan(false);
    ble_scan->clearResults(); // delete results fromBLEScan buffer to release memory
}
