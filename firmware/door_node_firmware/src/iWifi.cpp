#include <iWifi.h>
#include <WiFi.h>

/* Class constructor */
iWifi::iWifi(const char *ssid, const char *password)
{
  _ssid = ssid;
  _password = password;
}

/* Class destructor */
iWifi::~iWifi() {}

void iWifi::connect()
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