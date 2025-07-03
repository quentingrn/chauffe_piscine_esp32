#include "web.h"
#include "sensors.h"
#include "chauffage.h"
#include "FS.h"
#include "SPIFFS.h"
#include "utils.h"

WebManager::WebManager(SensorsManager* sensorsRef, ChauffageManager* chauffageRef)
    : sensors(sensorsRef), chauffage(chauffageRef) {}

void WebManager::begin() {
    server.on("/", [this]() { handleRoot(); });
    server.on("/on", [this]() { handleOn(); });
    server.on("/off", [this]() { handleOff(); });
    server.on("/history.csv", [this]() { handleHistory(); });
    server.begin();
}

void WebManager::handleClient() {
    server.handleClient();
}

void WebManager::handleRoot() {
    String page = "<html><head><script src='https://cdn.jsdelivr.net/npm/chart.js'></script></head><body>";
    page += "<h1>Chauffe Piscine</h1>";
    page += "<p>Eau: " + Utils::formatFloat(sensors->getWaterTemp()) + "&deg;C<br>";
    page += "Air: " + Utils::formatFloat(sensors->getAirTemp()) + "&deg;C<br>";
    page += "Etat chauffage: " + String(chauffage->isOn() ? "ON" : "OFF") + "</p>";
    page += "<a href='/on'>ON</a> | <a href='/off'>OFF</a><br><br>";
    page += "<canvas id='chart' width='300' height='200'></canvas>";
    page += "<script>\n";
    page += "fetch('/history.csv').then(r=>r.text()).then(t=>{const lines=t.trim().split('\\n').slice(1);const labels=[];const water=[];const air=[];lines.forEach(l=>{const p=l.split(',');labels.push(p[0]);water.push(parseFloat(p[1]));air.push(parseFloat(p[2]));});new Chart(document.getElementById('chart'),{type:'line',data:{labels:labels,datasets:[{label:'Eau',data:water,borderColor:'blue',fill:false},{label:'Air',data:air,borderColor:'red',fill:false}]}});});";
    page += "</script>";
    page += "</body></html>";
    server.send(200, "text/html", page);
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

