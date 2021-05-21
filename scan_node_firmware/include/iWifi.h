#include <Arduino.h>
#include <vector>

#define MAX_CHANNEL 13 //max Channel -> US = 11, EU = 13, Japan = 14
#define DEFAULT_TTL 24
#define FIRST_CHANNEL 1

class iWifi
{
private:
  struct device
  {
    unsigned int id;
    String mac;
    int ttl;
  };

  const char *_ssid;
  const char *_password;
  unsigned int _current_channel;
  std::vector<device> _devices_list;
  std::vector<String> know_devices = {
      "ac18262dfe49",
      "02404b44519b",
      "982cbce84e37",
      "147dda9a7d55",
      "78abbb00df8f",
      "0013719fc5e4",
      "9c9c1fcb0624",
      "0e89055493b0",
      "9c9c1fcb0624",
      "9c9c1fca7c64",
      "48bf6bdbace2",
      "dc91bf997e00",
      "88cb87ee6957",
      "000c2962aebe",
      "4cbb5859898a",
      "38bc1b93a2c0"
    };

public:
  iWifi(const char *ssid, const char *password);
  ~iWifi();
  void set_channel(unsigned channel_to_change);
  unsigned int get_channel();
  unsigned int get_devices_number();
  String get_dev_mac(unsigned int index);
  void set_dev_ttl(unsigned int index);
  /* STA MODE */
  void connect_wifi();
  boolean there_are_dev(int devs_number);
  void check_max_channel();
  void add_device(unsigned int id, String mac, int ttl);
  void purge_devices();
  void show_devices();
  boolean is_know_device(String mac_dev);
  String format_mac(IPAddress ip);
  boolean check_equals_macs(String mac1, String mac2);
};