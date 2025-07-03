#ifndef WEB_H
#define WEB_H

#include <Arduino.h>
#include <WebServer.h>

class Sensors;
class Chauffage;

class WebServerApp {
public:
    WebServerApp(Sensors* sensorsRef, Chauffage* chauffageRef);
    void begin();
    void handleClient();

private:
    void handleRoot();
    void handleOn();
    void handleOff();

    WebServer server{80};
    Sensors* sensors;
    Chauffage* chauffage;
};

#endif // WEB_H
