#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <vector>

#define MAX_CHANNEL 13 //max Channel -> US = 11, EU = 13, Japan = 14

/* Device */
struct device
{
  unsigned int id;
  String mac;
  int ttl;
  String state;
};

class iWifi
{
private:
  const char *_ssid;
  const char *_password;
  unsigned int _current_channel;
  std::vector<device> _devices_list;


public:
  std::vector<String> know_devices = {
      "ac18262dfe49",
      "982cbce84e37",
      "147dda9a7d55",
      "78abbb00df8f",
      "0013719fc5e4",
      "4cbb5859898a"
      "9c9c1fcb0624",
    };

  iWifi(const char *ssid, const char *password);
  ~iWifi();
  void set_channel(unsigned channel_to_change);
  unsigned int get_channel();
  unsigned int get_devices_number();
  String get_dev_mac(unsigned int index);
  /* STA MODE */
  void connect_wifi();
  boolean there_are_dev(int devs_number);
  void check_max_channel();
  void add_device(unsigned int id, String mac, int ttl, String state);
  void update_devices();
  void show_people();
  boolean is_know_device(String mac_dev);
  String format_mac(IPAddress ip);
};