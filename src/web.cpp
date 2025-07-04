#include "web.h"
#include "sensors.h"
#include "chauffage.h"
#include "FS.h"
#include "SPIFFS.h"
#include "utils.h"

WebManager::WebManager(SensorsManager* sensorsRef, ChauffageManager* chauffageRef)
    : sensors(sensorsRef), chauffage(chauffageRef) {}

void WebManager::begin() {
    server.on("/", HTTP_GET, std::bind(&WebManager::handleRoot, this));
    server.on("/", HTTP_POST, std::bind(&WebManager::handleSet, this));
    server.on("/on", HTTP_GET, std::bind(&WebManager::handleOn, this));
    server.on("/off", HTTP_GET, std::bind(&WebManager::handleOff, this));
    server.on("/history.csv", HTTP_GET, std::bind(&WebManager::handleHistory, this));
    server.begin();
}

void WebManager::handleClient() {
    server.handleClient();
}

void WebManager::handleRoot() {
    ChauffageManager::Schedule sch = chauffage->getSchedule();
    String page = "<html><body><h1>Chauffe Piscine</h1>";
    page += "<p>Eau: " + Utils::formatFloat(sensors->getWaterTemp()) + "&deg;C<br>";
    page += "Air: " + Utils::formatFloat(sensors->getAirTemp()) + "&deg;C<br>";
    page += "Etat chauffage: " + String(chauffage->isOn() ? "ON" : "OFF") + "</p>";
    page += "<form method='post'>";
    page += "Mode: <select name='mode'>";
    page += "<option value='MANUEL'";
    if (chauffage->getMode() == ChauffageManager::MANUEL) page += " selected";
    page += ">MANUEL</option>";
    page += "<option value='AUTO'";
    if (chauffage->getMode() == ChauffageManager::AUTO) page += " selected";
    page += ">AUTO</option></select><br>";
    page += "Temp cible: <input type='number' step='0.1' name='target' value='" + String(chauffage->getTargetTemp(),1) + "'><br>";
    page += "<input type='checkbox' name='schedule'";
    if (sch.enabled) page += " checked";
    page += ">Utiliser plage horaire<br>";
    page += "Début: <input type='time' name='start' value='" + Utils::formatTime(sch.startHour, sch.startMin) + "'><br>";
    page += "Fin: <input type='time' name='end' value='" + Utils::formatTime(sch.endHour, sch.endMin) + "'><br>";
    page += "<input type='submit' value='Sauver'></form>";
    if (chauffage->getMode() == ChauffageManager::MANUEL) {
        page += "<p><a href='/on'>FORCER ON</a> | <a href='/off'>FORCER OFF</a></p>";
    }
    page += "<p><a href='/history.csv'>Historique</a></p>";
    page += "</body></html>";
    server.send(200, "text/html", page);
}

void WebManager::handleSet() {
    if (server.hasArg("mode")) {
        String m = server.arg("mode");
        if (m == "MANUEL") chauffage->setMode(ChauffageManager::MANUEL);
        else chauffage->setMode(ChauffageManager::AUTO);
    }
    if (server.hasArg("target")) {
        chauffage->setTargetTemp(server.arg("target").toFloat());
    }
    ChauffageManager::Schedule sch = chauffage->getSchedule();
    sch.enabled = server.hasArg("schedule");
    if (server.hasArg("start")) {
        sscanf(server.arg("start").c_str(), "%2hhu:%2hhu", &sch.startHour, &sch.startMin);
    }
    if (server.hasArg("end")) {
        sscanf(server.arg("end").c_str(), "%2hhu:%2hhu", &sch.endHour, &sch.endMin);
    }
    chauffage->setSchedule(sch);
    server.sendHeader("Location", "/", true);
    server.send(303, "text/plain", "");
}

void WebManager::handleOn() {
    chauffage->manualOn();
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void WebManager::handleOff() {
    chauffage->manualOff();
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void WebManager::handleHistory() {
    File f = SPIFFS.open("/history.csv", FILE_READ);
    if (f) {
        server.streamFile(f, "text/csv");
        f.close();
    } else {
        server.send(404, "text/plain", "Not found");
    }
}
