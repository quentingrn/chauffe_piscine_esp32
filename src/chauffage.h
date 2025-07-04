#ifndef CHAUFFAGE_H
#define CHAUFFAGE_H

#include <Arduino.h>
#include <Servo.h>

#include "config.h"
#include "utils.h"

class SensorsManager; // forward declaration

// Controls the heating servo and automation modes
class ChauffageManager {
public:
    enum Mode { MANUEL, AUTO };

    struct Schedule {
        bool enabled = false;
        uint8_t startHour = 0;
        uint8_t startMin = 0;
        uint8_t endHour = 23;
        uint8_t endMin = 59;
    };

    explicit ChauffageManager(SensorsManager* sensorsRef) : sensors(sensorsRef) {}

    void begin();
    void update();

    void manualOn();
    void manualOff();

    void setMode(Mode m) { mode = m; }
    Mode getMode() const { return mode; }

    void setTargetTemp(float t) { targetTemp = t; }
    float getTargetTemp() const { return targetTemp; }

    void setSchedule(const Schedule& s) { schedule = s; }
    Schedule getSchedule() const { return schedule; }

    bool isOn() const { return heating; }

private:
    bool withinSchedule(const tm& t) const;
    void applyHeating(bool on);
    void logState();

    SensorsManager* sensors;
    Servo servo;
    bool heating = false;
    Mode mode = DEFAULT_MODE == MODE_AUTO ? AUTO : MANUEL;
    float targetTemp = TARGET_TEMP_DEFAULT;
    Schedule schedule{};
    unsigned long lastLog = 0;
};

#endif // CHAUFFAGE_H
