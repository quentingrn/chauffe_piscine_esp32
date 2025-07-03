#ifndef CHAUFFAGE_H
#define CHAUFFAGE_H

#include <Arduino.h>
#include <Servo.h>

class Sensors; // forward declaration

class Chauffage {
public:
    explicit Chauffage(Sensors* sensorsRef) : sensors(sensorsRef) {}

    void begin();
    void update();

    void manualOn();
    void manualOff();
    void setAutoMode(bool enabled) { autoMode = enabled; }
    bool isOn() const { return heating; }

private:
    Sensors* sensors;
    Servo servo;
    bool heating = false;
    bool autoMode = true;
    float targetTemp = 28.0f; // target water temperature
};

#endif // CHAUFFAGE_H
