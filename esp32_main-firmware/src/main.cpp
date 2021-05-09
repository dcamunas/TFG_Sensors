#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertisedDevice.h>
#include <BLEScan.h>

#include <definitions.h>
#include <vector>

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
BLEScan *ble_scan;

unsigned long send_time;

void connect_wifi();
void reconnect();
void send_mqtt_data();
void setup_ble();
void scan_ble(boolean &send_mqtt);
String format_mac(IPAddress ip);

void setup()
{
  Serial.begin(BAUD_RATE);
  setup_ble();
}

void loop()
{
  boolean send_mqtt = false;

  scan_ble(send_mqtt);

  if (send_mqtt)
  {
    //my_wifi.show_people();
    send_mqtt_data();
  }
  delay(SCAN_TIME * 1000);
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
  connect_wifi();
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
  ble_scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  ble_scan->setActiveScan(true); //active scan uses more power, but get results faster
  ble_scan->setInterval(100);
  ble_scan->setWindow(99); // less or equal setInterval value
}

void scan_ble(boolean &send_mqtt)
{
  BLEScanResults found_devices = ble_scan->start(SCAN_TIME, false);
  Serial.println(found_devices.getCount());
  Serial.println("Scan done!");
  ble_scan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}

void connect_wifi()
{
  delay(250);
  Serial.println("[X] Connecting to " + String(SSID));
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[X] Connected to " + String(SSID) + " as: " + format_mac(WiFi.localIP()));
}

String format_mac(IPAddress ip)
{
  String str = "";
  unsigned int i;
  for (i = 0; i < 4; i++)
    str += i ? "." + String(ip[i]) : String(ip[i]);
  return str;
}