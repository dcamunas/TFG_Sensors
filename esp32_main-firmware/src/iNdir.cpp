#include <Arduino.h>
#include <iNdir.h>

iNdir::iNdir() {}
iNdir::~iNdir() {}
int iNdir::get_co2_value() { return _concentration; }

void iNdir::read_value(int sensor_value)
{
    _voltage = sensor_value * (3300 / 4095.0);

    /* Check preheating */
    if (_voltage == MIN_VOLTAGE)
    {
        Serial.println("Fault");
    }
    else if (_voltage < MAX_VOLTAGE)
    {
        Serial.println("Preheating");
    }
    else
    {
        _voltage_dif = _voltage - MAX_VOLTAGE;
        _concentration = (_voltage_dif * 50.0) / 16.0;
    }
}