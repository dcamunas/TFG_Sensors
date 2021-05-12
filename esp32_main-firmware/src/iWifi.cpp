#include <iWifi.h>
#include <WiFi.h>
#include <vector>

/* Class constructor */
iWifi::iWifi(const char *ssid, const char *password)
{
  _ssid = ssid;
  _password = password;
  _current_channel = 1;
}

/* Class destructor */
iWifi::~iWifi() {}

void iWifi::set_channel(unsigned int channel_to_change)
{
  _current_channel = channel_to_change;
}

unsigned int iWifi::get_channel() { return _current_channel; }
unsigned int iWifi::get_devices_number() { return _devices_list.size(); }
String iWifi::get_dev_mac(unsigned int index){return _devices_list.at(index).mac;}
void iWifi::set_dev_ttl(unsigned int index){_devices_list.at(index).ttl = DEFAULT_TTL;}

void iWifi::connect_wifi()
{
  delay(250);
  Serial.print("[X] Connecting to " + String(_ssid));
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

void iWifi::show_devices()
{
  for (int i = 0; i < _devices_list.size(); i++)
  {
    if (_devices_list[i].mac != "")
    {
      Serial.println("[X] DEVICE ID: " + String(_devices_list[i].id) + " || MAC ADR: " + _devices_list[i].mac + " || TTL: " + _devices_list[i].ttl);
    }
  }
  Serial.println("------------------------------------------------\n \t[X] Totals Devices: " + String(_devices_list.size()));
  Serial.println("------------------------------------------------------------------------------------------------\n\n");
}

/* --------------------------------------------------------------------- */

boolean iWifi::is_know_device(String mac_dev)
{
  boolean is_know = false;
  for (unsigned int i = 0; i < know_devices.size() && !is_know; i++)
  {
    is_know = check_equals_macs(mac_dev, know_devices.at(i));
  }
  return is_know;
}

void iWifi::check_max_channel()
{
  if (_current_channel > MAX_CHANNEL)
    _current_channel = FIRST_CHANNEL;
}

void iWifi::add_device(unsigned int id, String mac, int ttl){_devices_list.push_back({id, mac, ttl});}

/*void iWifi::purge_devices()
{
  for (unsigned int i = 0; i < _devices_list.size(); i++)
  {
    _devices_list[i].ttl--;

    if (_devices_list[i].ttl <= 0)
      _devices_list.erase(_devices_list.begin() + i);
  }
}*/

void iWifi::purge_devices(){_devices_list.clear();}

/* Checking of there are device in the room */
boolean iWifi::there_are_dev(int length)
{
  return !(length < 0);
}

boolean iWifi::check_equals_macs(String mac1, String mac2)
{
  mac1 = mac1.substring(5, 5+5);
  mac2 = mac2.substring(mac2.indexOf(mac1), mac2.indexOf(mac1)+5);

  return mac1 == mac2;
}
