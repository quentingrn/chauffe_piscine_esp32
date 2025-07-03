#include "chauffage.h"
#include "sensors.h"

static constexpr uint8_t SERVO_PIN = 15; // GPIO controlling the switch

void Chauffage::begin() {
    servo.attach(SERVO_PIN);
}

void Chauffage::manualOn() {
    heating = true;
    servo.write(180); // arbitrary position for ON
}

void Chauffage::manualOff() {
    heating = false;
    servo.write(0); // position for OFF
}

void Chauffage::update() {
    if (!sensors) {
        return;
    }

    if (autoMode) {
        float water = sensors->getWaterTemp();
        if (!isnan(water)) {
            if (water < targetTemp && !heating) {
                manualOn();
            } else if (water >= targetTemp && heating) {
                manualOff();
            }
        }
    }
}

