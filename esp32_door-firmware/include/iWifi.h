#include <Arduino.h>

class iWifi
{
private:
  const char *_ssid;
  const char *_password;

public:
  iWifi(const char *ssid, const char *password);
  ~iWifi();
  void connect();
  String format_mac(IPAddress ip);
};