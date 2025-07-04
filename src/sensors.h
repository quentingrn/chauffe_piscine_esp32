#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

// Manages the DS18B20 sensors (water and air)
class SensorsManager {
public:
    void begin();
    void update();

    float getWaterTemp() const { return waterTemp; }
    float getAirTemp() const { return airTemp; }

private:
    static constexpr uint8_t ONE_WIRE_BUS = ONE_WIRE_PIN; // GPIO pin from config

    OneWire oneWire{ONE_WIRE_BUS};
    DallasTemperature ds{&oneWire};
    DeviceAddress waterAddr{};
    DeviceAddress airAddr{};

    float waterTemp = NAN;
    float airTemp = NAN;
};

#endif // SENSORS_H
