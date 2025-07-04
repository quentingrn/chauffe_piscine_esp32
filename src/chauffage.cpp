#include "chauffage.h"
#include "sensors.h"
#include "FS.h"
#include "SPIFFS.h"

void ChauffageManager::begin() {
    servo.attach(PIN_SERVO);
}

void ChauffageManager::manualOn() {
    applyHeating(true);
}

void ChauffageManager::manualOff() {
    applyHeating(false);
}

bool ChauffageManager::withinSchedule(const tm& t) const {
    if (!schedule.enabled) return true;
    int current = t.tm_hour * 60 + t.tm_min;
    int start = schedule.startHour * 60 + schedule.startMin;
    int end = schedule.endHour * 60 + schedule.endMin;
    if (start <= end) {
        return current >= start && current < end;
    }
    return current >= start || current < end;
}

void ChauffageManager::applyHeating(bool on) {
    if (heating == on) return;
    heating = on;
    servo.write(on ? 180 : 0);
    logState();
}

void ChauffageManager::update() {
    if (!sensors) return;

    time_t raw;
    time(&raw);
    tm timeinfo;
    localtime_r(&raw, &timeinfo);

    if (!withinSchedule(timeinfo)) {
        if (heating) applyHeating(false);
        return;
    }

    if (mode == AUTO) {
        float water = sensors->getWaterTemp();
        if (!isnan(water)) {
            if (water < targetTemp) {
                applyHeating(true);
            } else {
                applyHeating(false);
            }
        }
    }
}

void ChauffageManager::logState() {
    File f = SPIFFS.open("/chauffage.csv", FILE_APPEND);
    if (f) {
        f.printf("%s,%s\n", Utils::timestamp().c_str(), heating ? "ON" : "OFF");
        f.close();
    }
}
