#include <Arduino.h>
#include <PubSubClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <vector>
#include <WiFi.h>
#include <esp_wifi.h>
#include <definitions.h>
#include <iWifi.h>
#include <iNdir.h>
#include <utils.h>

/* Global variables */
WiFiClient esp_client;
PubSubClient client(esp_client);
iWifi my_wifi(SSID, PASSWORD);
BLEScan *ble_scan;
iNdir my_ndir;

unsigned int ble_devs_count, old_ble_devs_count, old_wifi_devs_count;
float old_co2_value;
unsigned long send_time;

void sniffer(void *buffer, wifi_promiscuous_pkt_type_t packet_type);
void reconnect();
void setup_ble();
void setup_promiscuous_mode();
void scan_wifi(boolean &send_mqtt);
void scan_ble(boolean &send_mqtt);
void send_mqtt_data();
boolean check_send_time();
boolean check_co2_level();

void setup()
{
  ble_devs_count = old_ble_devs_count = old_wifi_devs_count = 0;
  send_time = 0;
  old_co2_value = 0.00;

  Serial.begin(BAUD_RATE);

  my_wifi.connect_wifi();
  setup_ble();
  client.setServer(MQTT_SERVER_VM, MOSQUITTO_PORT);
  setup_promiscuous_mode();

  pinMode(CO2_PIN, INPUT);
}

void loop()
{
    boolean send_mqtt = false;

    scan_wifi(send_mqtt);
    scan_ble(send_mqtt);
    my_ndir.read_value(analogRead(CO2_PIN));
    send_mqtt += check_co2_level() + check_send_time();

    //if (send_mqtt)
      send_mqtt_data();

    esp_wifi_set_channel(my_wifi.get_channel(), WIFI_SECOND_CHAN_NONE);
    my_wifi.set_channel(my_wifi.get_channel() + 1);
    delay(5000);
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
  for (i = 4; i <= 15; i++)
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
  my_wifi.purge_devices();
  my_wifi.check_max_channel();
  esp_wifi_set_channel(my_wifi.get_channel(), WIFI_SECOND_CHAN_NONE);

  if (my_wifi.get_devices_number() != old_wifi_devs_count)
  {
    old_wifi_devs_count = my_wifi.get_devices_number();
    send_mqtt += true;
  }
}

void reconnect()
{
  while (!client.connected())
  {
    if (!client.connect("esp32_main"))
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(2500);    
    }
    yield();
  }
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
  ble_devs_count = found_devices.getCount();
  ble_scan->setActiveScan(false);
  ble_scan->clearResults();

  if (ble_devs_count != old_ble_devs_count)
  {
    old_ble_devs_count = ble_devs_count;
    send_mqtt += true;
  }
}

void send_mqtt_data()
{
  esp_wifi_set_promiscuous(false);

  if (!client.connected())
    reconnect();
  client.loop();

  client.publish(scan_topic.c_str(), line_protocol(old_ble_devs_count, old_wifi_devs_count, old_co2_value).c_str());
  
  client.disconnect();

  send_time = millis();
  esp_wifi_set_promiscuous(true);
}

boolean check_send_time()
{
  return (millis() - send_time) >= MAX_SEND_TIME;
}

boolean check_co2_level()
{
  if (my_ndir.get_co2_value() != old_co2_value && (my_ndir.get_co2_value() > MAX_CO2_VALUE && my_ndir.get_co2_value() < MIN_CO2_VALUE))
  {
    old_co2_value = my_ndir.get_co2_value();
    return true;
  }
  return false;
}