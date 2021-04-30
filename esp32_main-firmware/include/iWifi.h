#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
//#include <esp_wifi.h>
#include <vector>

#define MAX_CHANNEL 13 //max Channel -> US = 11, EU = 13, Japan = 14

/* Device */
struct device
{
    int id;
    String mac;
    int ttl;
    String state;
};

/* Mac address */
typedef struct
{
  String mac;
} __attribute__((packed)) mac_address;

/* Packet header's data */
typedef struct
{
  int16_t fctl;
  int16_t duration;
  mac_address da;
  mac_address sa;
  mac_address bssid;
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed)) wifi_mgmt_headder;


/* Promiscuous Filter's mask */
const wifi_promiscuous_filter_t filter = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};


class iWifi
{
private:
    const char *_ssid;
    const char *_password;
    boolean _enable_promis_mode;
    WiFiClient _client;
    unsigned int _current_channel;

public:
    std::vector<device> devices_list;

    iWifi(const char *ssid, const char *password, WiFiClient &client);
    ~iWifi();
    void set_channel(unsigned channel_to_change);
    unsigned int get_channel();
    unsigned int get_devices_number();
    /* STA MODE */
    void setup_sta_mode();
    /* PROMISCOUS MODE */
    void setup_promiscuous_mode();
    void sniffer(void *buffer, wifi_promiscuous_pkt_type_t packet_type);
    void start_promiscuous_mode();
    boolean there_are_dev(int devs_number);
    void check_max_channel();
    void update_ttl();
    void show_people();
    String ip_to_str(IPAddress ip);
};