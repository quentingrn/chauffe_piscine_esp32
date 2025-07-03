#include "sensors.h"

void Sensors::begin() {
    ds.begin();
    if (ds.getDeviceCount() >= 2) {
        ds.getAddress(waterAddr, 0);
        ds.getAddress(airAddr, 1);
    }
}

void Sensors::update() {
    ds.requestTemperatures();
    waterTemp = ds.getTempC(waterAddr);
    airTemp = ds.getTempC(airAddr);
}

