#include "chauffage.h"
#include "sensors.h"
#include "FS.h"
#include "SPIFFS.h"

static constexpr uint8_t SERVO_PIN = 14; // GPIO controlling the switch

void ChauffageManager::begin() {
    servo.attach(SERVO_PIN);
}

void ChauffageManager::setMode(Mode m) {
    mode = m;
    switch (mode) {
        case AUTO_ADULTE:
            targetTemp = 33.0f;
            break;
        case AUTO_ENFANT:
            targetTemp = 29.0f;
            break;
        default:
            break;
    }
}

void ChauffageManager::manualOn() {
    heating = true;
    servo.write(180); // arbitrary position for ON
    logState();
}

void ChauffageManager::manualOff() {
    heating = false;
    servo.write(0); // position for OFF
    logState();
}

void ChauffageManager::update() {
    if (!sensors) {
        return;
    }

    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    if (mode != MANUEL) {
        // Determine target temperature based on mode and schedule
        if (mode == AUTO_ADULTE && (timeinfo.tm_hour < 17 || timeinfo.tm_hour >= 21)) {
            manualOff();
            return;
        }
        if (mode == AUTO_ENFANT && (timeinfo.tm_hour < 12 || timeinfo.tm_hour >= 17)) {
            manualOff();
            return;
        }

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

void ChauffageManager::logState() {
    unsigned long now = millis();
    if (now - lastLog < 1000) return; // debounce
    lastLog = now;
    File f = SPIFFS.open("/chauffage.csv", FILE_APPEND);
    if (f) {
        time_t t;
        time(&t);
        struct tm *tm_info = localtime(&t);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        f.printf("%s,%s\n", buf, heating ? "ON" : "OFF");
        f.close();
    }
}

