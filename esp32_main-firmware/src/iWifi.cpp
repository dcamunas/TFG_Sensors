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
  _devices_number = 0;
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

  Serial.println("\n[X] Connected to " + String(_ssid) + " as: " + format_mac(WiFi.localIP()));
}

String iWifi::format_mac(IPAddress ip)
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

bool iWifi::is_know_device(String mac_dev)
{
  for (unsigned int i = 0; i < know_devices.size(); i++)
  {
    if (mac_dev == know_devices.at(i))
      return true;
  }
  return false;
}

void iWifi::check_max_channel()
{
  if (_current_channel > MAX_CHANNEL)
    _current_channel = 1;
}

void iWifi::update_ttl()
{
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
