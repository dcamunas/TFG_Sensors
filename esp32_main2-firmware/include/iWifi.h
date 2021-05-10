#include <Arduino.h>
#include <WiFi.h>

class iWifi
{
private:
    const char *_ssid;
    const char *_password;

    String _format_mac(IPAddress ip);

public:
    iWifi(const char *ssid, const char *password);
    ~iWifi();
    void connect_wifi();
};

iWifi::iWifi(const char *ssid, const char *password)
{
    _ssid = ssid;
    _password = password;
}

iWifi::~iWifi() {}

void iWifi::connect_wifi()
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

    Serial.println("\n[X] Connected to " + String(_ssid) + " as: " + _format_mac(WiFi.localIP()));
}

String iWifi::_format_mac(IPAddress ip)
{
    String str = "";
    unsigned int i;
    for (i = 0; i < 4; i++)
        str += i ? "." + String(ip[i]) : String(ip[i]);
    return str;
}