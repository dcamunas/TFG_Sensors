#include <BLEAdvertisedDevice.h>

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};