#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Manages the DS18B20 sensors (water and air)
class SensorsManager {
public:
    void begin();
    void update();

    float getWaterTemp() const { return waterTemp; }
    float getAirTemp() const { return airTemp; }

private:
    static constexpr uint8_t ONE_WIRE_BUS = 4; // GPIO pin for DS18B20 bus

    OneWire oneWire{ONE_WIRE_BUS};
    DallasTemperature ds{&oneWire};
    DeviceAddress waterAddr{};
    DeviceAddress airAddr{};

    float waterTemp = NAN;
    float airTemp = NAN;
};

#endif // SENSORS_H
