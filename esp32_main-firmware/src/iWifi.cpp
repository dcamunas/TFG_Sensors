#include <iWifi.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_wifi.h>

#include <colors.h>
#include <vector>

/* Class constructor */
iWifi::iWifi(const char *ssid, const char *password, WiFiClient &client)
{
    _ssid = ssid;
    _password = password;
    _enable_promis_mode = false;
    _client = client;
    _current_channel = 1;
    //_devices_number = 0;
}

/* Class destructor */
iWifi::~iWifi() {}

void iWifi::set_mode(boolean condition)
{
    _enable_promis_mode = condition;
}

boolean iWifi::get_mode() { return _enable_promis_mode; }

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

    for (i = 0; i < _devices_list.size() && new_device; i++)
        if (mac_dev == _devices_list[i].mac)
            new_device = false;

    // If device is new in the room
    if (new_device)
        _devices_list.size() == 0 ?
            _devices_list.push_back({0, mac_dev, 64, ""}) : _devices_list.push_back({int(_devices_list.size() - 1), mac_dev, 64, ""});
}   

void iWifi::setup_promiscuous_mode()
{
    delay(250);
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();
}

String iWifi::ip_to_str(IPAddress ip){
  String str="";
  unsigned int i;
  for (i=0; i<4; i++)
    str += i  ? "." + String(ip[i]) : String(ip[i]);
  return str;
}