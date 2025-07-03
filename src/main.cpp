#include <Arduino.h>
#include "sensors.h"
#include "chauffage.h"
#include "web.h"
#include "utils.h"
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <time.h>

static const char* WIFI_SSID = "YOUR_SSID";
static const char* WIFI_PASS = "YOUR_PASSWORD";

SensorsManager sensors;
ChauffageManager chauffage(&sensors);
WebManager web(&sensors, &chauffage);

unsigned long lastHistory = 0;

void setup() {
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
    }

    configTime(0, 0, "pool.ntp.org");

    sensors.begin();
    chauffage.begin();
    web.begin();
}

void loop() {
    sensors.update();
    chauffage.update();
    web.handleClient();

    unsigned long now = millis();
    if (now - lastHistory >= 60000) {
        lastHistory = now;
        File f = SPIFFS.open("/history.csv", FILE_APPEND);
        if (f) {
            f.printf("%s,%.2f,%.2f,%d\n", Utils::timestamp().c_str(), sensors.getWaterTemp(), sensors.getAirTemp(), chauffage.isOn());
            f.close();
        }
    }
}

