#include <Arduino.h>
#include "sensors.h"
#include "chauffage.h"
#include "web.h"
#include "utils.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <FS.h>
#include <SPIFFS.h>
#include <time.h>

// Enable or disable debug output on the Serial monitor
#ifndef DEBUG_MODE
#define DEBUG_MODE 1
#endif


SensorsManager sensors;
ChauffageManager chauffage(&sensors);
WebManager web(&sensors, &chauffage);

unsigned long lastHistory = 0;
unsigned long lastWifiRetry = 0;
unsigned long wifiRetryDelay = 1000;
unsigned long lastInternetProbe = 0;
unsigned long lastStatusPrint = 0;
bool internetOk = false;
unsigned long noNetworkStart = 0;

Preferences prefs;
time_t lastRestart = 0;

void setup() {
    Serial.begin(115200);

    prefs.begin("sys", false);
    lastRestart = prefs.getULong("last_restart", 0);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        WiFi.setAutoReconnect(true);
        configTime(0, 0, "pool.ntp.org");
    }

    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
    }

    sensors.begin();
    chauffage.begin();
    web.begin();
}

void loop() {
    sensors.update();
    chauffage.update();
    web.handleClient();

    unsigned long now = millis();

    // WiFi reconnection with exponential backoff
    if (WiFi.status() != WL_CONNECTED) {
        if (now - lastWifiRetry >= wifiRetryDelay) {
            WiFi.reconnect();
            lastWifiRetry = now;
            if (wifiRetryDelay < 30000) {
                wifiRetryDelay = min(wifiRetryDelay * 2, 30000UL);
            }
        }
    } else {
        wifiRetryDelay = 1000;
    }

    // Internet reachability probe
    if (now - lastInternetProbe >= 60000) {
        lastInternetProbe = now;
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.setTimeout(5000);
            if (http.begin("http://clients3.google.com/generate_204")) {
                int code = http.GET();
                internetOk = (code == 204);
                http.end();
            } else {
                internetOk = false;
            }
        } else {
            internetOk = false;
        }
    }

    // Restart after 5 minutes without network
    bool networkOk = (WiFi.status() == WL_CONNECTED && internetOk);
    if (!networkOk) {
        if (noNetworkStart == 0) {
            noNetworkStart = now;
        } else if (now - noNetworkStart >= 300000) {
            time_t tnow;
            time(&tnow);
            if (lastRestart == 0 || tnow == 0 || tnow - lastRestart >= 600) {
                prefs.putULong("last_restart", tnow);
                Serial.println("[WARN] Restarting module: network unreachable");
                ESP.restart();
            } else {
                noNetworkStart = now; // avoid continuous restart attempts
            }
        }
    } else {
        noNetworkStart = 0;
    }

    // Periodic history logging
    if (now - lastHistory >= 60000) {
        lastHistory = now;
        File f = SPIFFS.open("/history.csv", FILE_APPEND);
        if (f) {
            f.printf("%s,%.2f,%.2f,%d\n", Utils::timestamp().c_str(), sensors.getWaterTemp(), sensors.getAirTemp(), chauffage.isOn());
            f.close();
        }

#if DEBUG_MODE
        const char* modeStr = (chauffage.getMode() == ChauffageManager::MANUEL) ? "MANUEL" : "AUTO";
        String line = String("MODE:") + modeStr +
                      " | Eau:" + Utils::formatFloat(sensors.getWaterTemp()) + "C" +
                      " Air:" + Utils::formatFloat(sensors.getAirTemp()) + "C" +
                      " | Servo:" + (chauffage.isOn() ? "ON" : "OFF");
        Serial.println(line);
#endif
    }

    // Status display every 30s
    if (now - lastStatusPrint >= 30000) {
        lastStatusPrint = now;
        String wifiInfo = (WiFi.status() == WL_CONNECTED)
                              ? "WiFi:" + WiFi.localIP().toString()
                              : "WiFi:DISCONNECTED";
        String netInfo = internetOk ? "Internet:OK" : "Internet:KO";
        Serial.println(wifiInfo + " | " + netInfo);
    }
}

