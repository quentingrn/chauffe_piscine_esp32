#include "web.h"
#include "sensors.h"
#include "chauffage.h"
#include "utils.h"

WebServerApp::WebServerApp(Sensors* sensorsRef, Chauffage* chauffageRef)
    : sensors(sensorsRef), chauffage(chauffageRef) {}

void WebServerApp::begin() {
    server.on("/", [this]() { handleRoot(); });
    server.on("/on", [this]() { handleOn(); });
    server.on("/off", [this]() { handleOff(); });
    server.begin();
}

void WebServerApp::handleClient() {
    server.handleClient();
}

void WebServerApp::handleRoot() {
    String page = "<html><body>";
    page += "Eau: " + Utils::formatFloat(sensors->getWaterTemp()) + "&deg;C<br>";
    page += "Air: " + Utils::formatFloat(sensors->getAirTemp()) + "&deg;C<br>";
    page += "Chauffage: " + String(chauffage->isOn() ? "ON" : "OFF") + "<br>";
    page += "<a href='/on'>ON</a> | <a href='/off'>OFF</a>";
    page += "</body></html>";
    server.send(200, "text/html", page);
}

void WebServerApp::handleOn() {
    chauffage->manualOn();
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void WebServerApp::handleOff() {
    chauffage->manualOff();
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

