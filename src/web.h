#ifndef WEB_H
#define WEB_H

#include <Arduino.h>
#include <WebServer.h>

class SensorsManager;
class ChauffageManager;

// Simple web interface using ESP32 WebServer
class WebManager {
public:
    WebManager(SensorsManager* sensorsRef, ChauffageManager* chauffageRef);
    void begin();
    void handleClient();

private:
    void handleRoot();
    void handleSet();
    void handleOn();
    void handleOff();
    void handleHistory();

    WebServer server{80};
    SensorsManager* sensors;
    ChauffageManager* chauffage;
};

#endif // WEB_H
