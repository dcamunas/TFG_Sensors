#include <Arduino.h>
#include <PubSubClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <vector>

#include <definitions.h>
#include <iWifi.h>
#include <utils.h>


/* Global variables */
WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);
BLEScan *ble_scan;

unsigned int ble_devices_number = 0, ble_old_devices_number = 0, wifi_old_devices_number = 0;
unsigned long send_time;

void sniffer(void *buffer, wifi_promiscuous_pkt_type_t packet_type);
void reconnect();
void setup_ble();
void setup_promiscuous_mode();
void scan_wifi(boolean &send_mqtt);
void scan_ble(boolean &send_mqtt);
void send_mqtt_data();

void setup()
{
  Serial.begin(BAUD_RATE);
  my_wifi.connect_wifi();
  setup_ble();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  setup_promiscuous_mode();
}

void loop()
{
  boolean send_mqtt = false;

  scan_wifi(send_mqtt);
  scan_ble(send_mqtt);

  if (send_mqtt)
  {
    send_mqtt_data();
  }
  esp_wifi_set_channel(my_wifi.get_channel(), WIFI_SECOND_CHAN_NONE);
  my_wifi.set_channel(my_wifi.get_channel() + 1);
  delay(SCAN_TIME * 1000);
}

/* ---------------------------------------------------------------------------------------------- */

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
  esp_wifi_set_promiscuous(false);
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_promiscuous(true);
}

void scan_wifi(boolean &send_mqtt)
{
   my_wifi.check_max_channel();
  esp_wifi_set_channel(my_wifi.get_channel(), WIFI_SECOND_CHAN_NONE);

  if (my_wifi.get_devices_number() != wifi_old_devices_number)
  {
    wifi_old_devices_number = my_wifi.get_devices_number();
    send_mqtt = true;
  }

  /*Para comprobar tiempo utilizar millis()*/
  if ((millis() - send_time) >= MAX_SEND_TIME)
    send_mqtt = true;

  my_wifi.purge_devices();
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(2500);
    }
    yield();
  }
}

void send_mqtt_data()
{
  String msg;
  esp_wifi_set_promiscuous(false);

  if (!client.connected())
    reconnect();
  client.loop();

  //msg = String("concentration, location=us, ppm=1");
  msg = "Wifi: " + String(wifi_old_devices_number) + "BLE: " + String(ble_devices_number);
  if (client.publish("test", msg.c_str()))
    Serial.println("Succesfully published.\n");
  else
    Serial.println("\nMessage not published\n");

  client.disconnect();

  send_time = millis();
  esp_wifi_set_promiscuous(true);
}

void setup_ble()
{
    BLEDevice::init("");
    ble_scan = BLEDevice::getScan();
    ble_scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    ble_scan->setActiveScan(true);
    ble_scan->setInterval(100);
    ble_scan->setWindow(99);
}

void scan_ble(boolean &send_mqtt)
{
    BLEScanResults found_devices = ble_scan->start(SCAN_TIME, false);
    ble_devices_number = found_devices.getCount();
    ble_scan->setActiveScan(false);
    ble_scan->clearResults();
}