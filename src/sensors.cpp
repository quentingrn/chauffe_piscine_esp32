#include "sensors.h"

void SensorsManager::begin() {
    ds.begin();
    if (ds.getDeviceCount() >= 2) {
        ds.getAddress(waterAddr, 0);
        ds.getAddress(airAddr, 1);
    }
}

void SensorsManager::update() {
    ds.requestTemperatures();
    waterTemp = ds.getTempC(waterAddr);
    airTemp = ds.getTempC(airAddr);
}

