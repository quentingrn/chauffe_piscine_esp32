#include <Arduino.h>
#include "sensors.h"
#include "chauffage.h"
#include "web.h"
#include "utils.h"

Sensors sensors;
Chauffage chauffage(&sensors);
WebServerApp web(&sensors, &chauffage);

void setup() {
    sensors.begin();
    chauffage.begin();
    web.begin();
}

void loop() {
    sensors.update();
    chauffage.update();
    web.handleClient();
}

