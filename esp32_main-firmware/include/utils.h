#include <esp_wifi.h>
#include <BLEAdvertisedDevice.h>
#include <vector>


/** WiFi utils **/
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


std::vector<String> ble_devices_list;
std::vector<String> know_ble_devices =
{
  "",
};
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        //Serial.printf("Address Device: %s \n", advertisedDevice.toString().c_str());
        //advertisedDevice.getAddress().toString();
        
    }
};