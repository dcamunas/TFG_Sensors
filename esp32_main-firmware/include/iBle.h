#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertisedDevice.h>
#include <BLEScan.h>

#define SCAN_TIME 2

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

class iBle
{
private:
    BLEScan *_ble_scan;
    unsigned int _devices_number;

public:
    iBle();
    ~iBle();
    void setup_ble();
    void scan_ble(boolean &send_mqtt);
    unsigned int get_devices_number();
};

iBle::iBle()
{
    BLEDevice::init("");
    _devices_number = 0;
}

iBle::~iBle() {}
unsigned int iBle::get_devices_number(){return _devices_number;}


void iBle::setup_ble()
{
    _ble_scan = BLEDevice::getScan(); //create new scan
    _ble_scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    _ble_scan->setActiveScan(true); //active scan uses more power, but get results faster
    _ble_scan->setInterval(100);
    _ble_scan->setWindow(99); // less or equal setInterval value
}

void iBle::scan_ble(boolean &send_mqtt)
{
    BLEScanResults found_devices = _ble_scan->start(SCAN_TIME, false);
    _devices_number = found_devices.getCount();
    Serial.println(_devices_number);
    Serial.println("Scan done!");
    _ble_scan->setActiveScan(false);
    _ble_scan->clearResults(); // delete results fromBLEScan buffer to release memory
}
