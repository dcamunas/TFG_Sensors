#include <iWifi.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_wifi.h>
#include <iostream>
#include <colors.h>
#include <vector>

/* Class constructor */
iWifi::iWifi(const char *ssid, const char *password)
{
  _ssid = ssid;
  _password = password;
  _enable_promis_mode = false;
  //_client = client;
  _current_channel = 1;
  //_devices_number = 0;
}

/* Class destructor */
iWifi::~iWifi() {}

void iWifi::set_channel(unsigned int channel_to_change)
{
  _current_channel = channel_to_change;
}

unsigned int iWifi::get_channel() { return _current_channel; }
unsigned int iWifi::get_devices_number() { return devices_list.size(); }

void iWifi::setup_sta_mode()
{
  delay(250);
  Serial.println("[X] Connecting to " + String(_ssid));
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[X] Connected to " + String(_ssid) + " as: " + ip_to_str(WiFi.localIP()));
}

String iWifi::ip_to_str(IPAddress ip)
{
  String str = "";
  unsigned int i;
  for (i = 0; i < 4; i++)
    str += i ? "." + String(ip[i]) : String(ip[i]);
  return str;
}

void iWifi::show_people()
{
  for (int i = 0; i < devices_list.size(); i++)
  {
    if (devices_list[i].mac != "")
    {
      Serial.println("[X] DEVICE ID: " + String(devices_list[i].id) + " || MAC ADR: " + devices_list[i].mac + " || TTL: " + devices_list[i].ttl + " || STATE: " + devices_list[i].state);
    }
  }
  Serial.println("------------------------------------------------\n \t[X] Totals Devices: " + String(devices_list.size()));
  Serial.println("------------------------------------------------------------------------------------------------\n\n");
}

/* --------------------------------------------------------------------- */

void iWifi::sniffer(void *buffer, wifi_promiscuous_pkt_type_t packet_type)
{
  String packet, mac_dev;
  unsigned int i, length = 0;
  boolean new_device = true;

  /* Gets payload and its lenght */
  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buffer;
  wifi_mgmt_headder *wh = (wifi_mgmt_headder *)pkt->payload;
  length = pkt->rx_ctrl.sig_len - sizeof(wh);

  if (!there_are_dev(length))
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
  mac_dev.toUpperCase();

  for (i = 0; i < devices_list.size() && new_device; i++)
    if (mac_dev == devices_list[i].mac)
      new_device = false;

  // If device is new in the room
  if (new_device)
    devices_list.size() == 0 ? devices_list.push_back({0, mac_dev, 64, ""}) : devices_list.push_back({int(devices_list.size() - 1), mac_dev, 64, ""});
}

void iWifi::setup_promiscuous_mode()
{
  delay(250);
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&config);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
}

// opcional
void iWifi::start_promiscuous_mode()
{
  // Activate promiscuous mode
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filter);
  //esp_wifi_set_promiscuous_rx_cb(&sniffer);

  esp_wifi_set_channel(_current_channel, WIFI_SECOND_CHAN_NONE);

  Serial.println("Scanning...");
}

void iWifi::check_max_channel()
{
  if (_current_channel > MAX_CHANNEL)
    _current_channel = 1;
}

void iWifi::update_ttl()
{
  int ttl;
  for (unsigned int i = 0; i < devices_list.size(); i++)
  {
    devices_list[i].ttl--;

    if (devices_list[i].ttl <= 0)
      devices_list.erase(devices_list.begin() + i);
  }
}

/* Checking of there are device in the room */
boolean iWifi::there_are_dev(int length)
{
  return !(length < 0);
}
