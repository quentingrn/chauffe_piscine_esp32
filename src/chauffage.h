#ifndef CHAUFFAGE_H
#define CHAUFFAGE_H

#include <Arduino.h>
#include <Servo.h>

class SensorsManager; // forward declaration

// Controls the heating servo and automation modes
class ChauffageManager {
public:
    enum Mode { MANUEL, AUTO_ADULTE, AUTO_ENFANT };

    explicit ChauffageManager(SensorsManager* sensorsRef) : sensors(sensorsRef) {}

    void begin();
    void update();

    void manualOn();
    void manualOff();
    void setMode(Mode m);
    Mode getMode() const { return mode; }
    bool isOn() const { return heating; }
    float getTargetTemp() const { return targetTemp; }

private:

    void logState();

    SensorsManager* sensors;
    Servo servo;
    bool heating = false;
    Mode mode = AUTO_ADULTE;
    float targetTemp = 33.0f; // default target temp for adult mode
    unsigned long lastLog = 0;
};

#endif // CHAUFFAGE_H
