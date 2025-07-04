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
    server.on("/set", HTTP_GET, std::bind(&WebManager::handleSet, this));
    server.on("/on", HTTP_GET, std::bind(&WebManager::handleOn, this));
    server.on("/off", HTTP_GET, std::bind(&WebManager::handleOff, this));
    server.on("/history.csv", HTTP_GET, std::bind(&WebManager::handleHistory, this));
    server.on("/chauffage.csv", HTTP_GET, std::bind(&WebManager::handleHeatingLog, this));
    server.begin();
}

void WebManager::handleClient() {
    server.handleClient();
}

void WebManager::handleRoot() {
    String page;
    page += "<!DOCTYPE html><html lang='fr'><head><meta charset='utf-8'>";
    page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    page += "<title>Chauffe Piscine</title>";
    page += "<style>body{background:#fff;color:#000;font-family:Arial,Helvetica,sans-serif;margin:0;padding:1rem;}";
    page += "form{max-width:400px;margin:auto;}fieldset{border:1px solid #000;padding:1rem;margin-bottom:1rem;}";
    page += "label{display:block;margin:.5rem 0;}#manual button{margin-right:.5rem;}";
    page += "</style></head><body>";
    page += "<h1>Chauffe Piscine</h1>";
    page += "<p>Eau: " + Utils::formatFloat(sensors->getWaterTemp()) + "&deg;C - Air: " + Utils::formatFloat(sensors->getAirTemp()) + "&deg;C - Chauffage: " + String(chauffage->isOn() ? "ON" : "OFF") + "</p>";
    page += "<form id='frm' action='/set' method='get'>";
    page += "<fieldset><legend>Mode</legend>";
    page += "<label><input type='radio' name='mode' value='MANUEL'";
    if (chauffage->getMode() == ChauffageManager::MANUEL) page += " checked";
    page += ">Manuel</label>";
    page += "<label><input type='radio' name='mode' value='AUTO'";
    if (chauffage->getMode() == ChauffageManager::AUTO) page += " checked";
    page += ">Auto</label></fieldset>";
    page += "<div id='manual'><button type='button' id='forceOn'>Forcer ON</button> <button type='button' id='forceOff'>Forcer OFF</button></div>";
    page += "<div id='auto'><label>Température cible:<input type='number' name='target' min='10' max='40' step='1' value='" + String((int)round(chauffage->getTargetTemp())) + "'></label></div>";
    page += "<button type='submit'>Sauver</button></form>";
    page += "<script>function update(){const m=document.querySelector('input[name=\"mode\"]:checked').value;document.getElementById('auto').style.display=m==='AUTO'?'block':'none';document.getElementById('manual').style.display=m==='MANUEL'?'block':'none';}";
    page += "document.querySelectorAll('input[name=\"mode\"]').forEach(el=>el.addEventListener('change',update));";
    page += "document.getElementById('forceOn').onclick=()=>location.href='/on';";
    page += "document.getElementById('forceOff').onclick=()=>location.href='/off';";
    page += "update();</script></body></html>";
    server.send(200, "text/html; charset=utf-8", page);
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

void WebManager::handleHeatingLog() {
    File f = SPIFFS.open("/chauffage.csv", FILE_READ);
    if (f) {
        server.streamFile(f, "text/csv");
        f.close();
    } else {
        server.send(404, "text/plain", "Not found");
    }
}
