#include <Arduino.h>
//#include <Wire.h>
#include <WiFi.h>
#include <esp_wifi.h>
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

unsigned int old_devices_number = 0;
unsigned long send_time;
BLEScan *ble_scan;

void sniffer(void *buffer, wifi_promiscuous_pkt_type_t packet_type);
void setup_promiscuous_mode();
void setup_ble();
void scan_wifi(boolean &send_mqtt);
void scan_ble(boolean &send_mqtt);
void reconnect();
void send_mqtt_data();

void setup()
{
  Serial.begin(BAUD_RATE);
  setup_promiscuous_mode();
  setup_ble();
}

void loop()
{
  boolean send_mqtt = false;

  scan_wifi(send_mqtt);
  //scan_ble(send_mqtt);

  Serial.println(send_mqtt);

  if (send_mqtt)
  {
    //my_wifi.show_people();
    send_mqtt_data();
  }
  //esp_wifi_set_channel(my_wifi.get_channel(), WIFI_SECOND_CHAN_NONE);
  my_wifi.set_channel(my_wifi.get_channel() + 1);
  delay(SCAN_TIME * 1000);
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

void sniffer(void *buffer, wifi_promiscuous_pkt_type_t packet_type)
{
  String packet, mac_dev;
  unsigned int i, length = 0;
  boolean new_device = true;

  /* Gets payload and its lenght */
  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buffer;
  wifi_mgmt_headder *wh = (wifi_mgmt_headder *)pkt->payload;
  length = pkt->rx_ctrl.sig_len - sizeof(wh);

  if (!my_wifi.there_are_dev(length))
  {
    Serial.println("[X] There are not devices in room.");
    return;
  }

  /* 
    This reads the first couple of bytes of the packet. 
    This is where you can read the whole packet replaceing the "8+6+1" (15), 
        with "p->rx_ctrl.sig_len"
    */
  for (i = 8; i < 16; i++)
    packet += String(pkt->payload[i], HEX);

  /*
    This removes the 'nibble' bits from the stat and end of the data we want. 
    So we only get the mac address.
    */
  for (i = 4; i < 16; i++)
    mac_dev += packet[i];
  //mac_dev.toUpperCase();

  if (!my_wifi.is_know_device(mac_dev))
  {
    for (i = 0; i < my_wifi.get_devices_number() && new_device; i++)
      if (mac_dev == my_wifi.get_dev_mac(i))
      {
        my_wifi.set_dev_ttl(i);
        new_device = false;
      }
    // If device is new in the room
    if (new_device)
      my_wifi.get_devices_number() == 0 ? my_wifi.add_device(0, mac_dev, DEFAULT_TTL) : my_wifi.add_device(my_wifi.get_devices_number() - 1, mac_dev, DEFAULT_TTL);
  }
}

void setup_promiscuous_mode()
{
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&config);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_channel(my_wifi.get_channel(), WIFI_SECOND_CHAN_NONE);

  //esp_wifi_start();

  esp_wifi_set_promiscuous(false);
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_promiscuous(true);
}

void scan_wifi(boolean &send_mqtt)
{
   my_wifi.check_max_channel();
  esp_wifi_set_channel(my_wifi.get_channel(), WIFI_SECOND_CHAN_NONE);

  if (my_wifi.get_devices_number() != old_devices_number)
  {
    old_devices_number = my_wifi.get_devices_number();
    send_mqtt = true;
  }

  /*Para comprobar tiempo utilizar millis()*/
  if ((millis() - send_time) >= MAX_SEND_TIME)
    send_mqtt = true;

  my_wifi.purge_devices();
  //my_wifi.show_people();
}

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
  esp_wifi_set_promiscuous(false);
  my_wifi.connect_wifi();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  if (!client.connected())
    reconnect();
  client.loop();

  //msg = String("concentration, location=us, ppm=1");
  msg = String(old_devices_number);
  if (client.publish("test", msg.c_str()))
    Serial.println("Succesfully published.\n");
  else
    Serial.println("\nMessage not published\n");

  client.disconnect();

  send_time = millis();
  esp_wifi_set_promiscuous(true);
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
