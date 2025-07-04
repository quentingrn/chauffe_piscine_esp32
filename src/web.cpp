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
    server.on("/chauffage.csv", HTTP_GET, std::bind(&WebManager::handleHeatingLog, this));
    server.begin();
}

void WebManager::handleClient() {
    server.handleClient();
}

void WebManager::handleRoot() {
    ChauffageManager::Schedule sch = chauffage->getSchedule();
    String page;
    page += "<!DOCTYPE html><html lang='fr'><head><meta charset='utf-8'>";
    page += "<title>Chauffe Piscine</title>";
    page += "<style>body{font-family:Arial,Helvetica,sans-serif;background:#fff;color:#000;margin:0;padding:10px;}";
    page += "section{border:1px solid #000;padding:10px;margin-bottom:20px;border-radius:4px;}";
    page += "h1{margin-top:0;}label{display:block;margin:5px 0;}input[type=number]{width:60px;}canvas{max-width:100%;height:auto;}";
    page += "@media(min-width:600px){body{max-width:800px;margin:auto;}}";
    page += "</style>";
    page += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
    page += "<script src='https://cdn.jsdelivr.net/npm/chartjs-adapter-date-fns'></script>";
    page += "</head><body>";
    page += "<h1>Chauffe Piscine</h1>";
    page += "<section id='etat'>";
    page += "<p>Eau: " + Utils::formatFloat(sensors->getWaterTemp()) + "&deg;C<br>";
    page += "Air: " + Utils::formatFloat(sensors->getAirTemp()) + "&deg;C<br>";
    page += "Chauffage: " + String(chauffage->isOn() ? "ON" : "OFF") + "</p></section>";
    page += "<section id='reglages'><form method='post'>";
    page += "<label>Mode:<select name='mode'>";
    page += "<option value='MANUEL'";
    if (chauffage->getMode() == ChauffageManager::MANUEL) page += " selected";
    page += ">MANUEL</option>";
    page += "<option value='AUTO'";
    if (chauffage->getMode() == ChauffageManager::AUTO) page += " selected";
    page += ">AUTO</option></select></label>";
    page += "<label>Température cible:<input type='number' name='target' step='1' min='10' max='35' value='" + String((int)round(chauffage->getTargetTemp())) + "'></label>";
    page += "<label><input type='checkbox' name='schedule'";
    if (sch.enabled) page += " checked";
    page += ">Utiliser plage horaire</label>";
    page += "<label>Début:<input type='time' name='start' value='" + Utils::formatTime(sch.startHour, sch.startMin) + "'></label>";
    page += "<label>Fin:<input type='time' name='end' value='" + Utils::formatTime(sch.endHour, sch.endMin) + "'></label>";
    page += "<input type='submit' value='Sauver'></form>";
    if (chauffage->getMode() == ChauffageManager::MANUEL) {
        page += "<p><a href='/on'>FORCER ON</a> | <a href='/off'>FORCER OFF</a></p>";
    }
    page += "</section>";
    page += "<section id='graph'><div><button id='btn24h'>24h</button> <button id='btn7d'>7j</button></div><canvas id='chart'></canvas></section>";
    page += "<script>let chart;";
    page += "function parseCSV(t){const l=t.trim().split('\n');l.shift();return l.map(e=>{const c=e.split(',');return{t:new Date(c[0]),w:parseFloat(c[1]),a:parseFloat(c[2]),h:c[3]==='1'||c[1]==='ON'}});}";
    page += "async function load(){const r=await fetch('/history.csv');const d=parseCSV(await r.text());return d;}";
    page += "function draw(data,h){const end=Date.now();const start=end-h*3600000;const f=data.filter(x=>x.t>=start);const lab=f.map(x=>x.t);const w=f.map(x=>x.w);const a=f.map(x=>x.a);const heat=f.map(x=>x.h?1:0);if(chart)chart.destroy();chart=new Chart(document.getElementById('chart'),{type:'line',data:{labels:lab,datasets:[{label:'Eau',data:w,borderColor:'blue',fill:false},{label:'Air',data:a,borderColor:'gray',fill:false},{label:'Chauffage',data:heat,type:'bar',backgroundColor:'rgba(255,0,0,0.3)',yAxisID:'y2'}]},options:{scales:{x:{type:'time',time:{unit:'hour'},ticks:{maxRotation:0,autoSkip:true}},y:{title:{display:true,text:'\u00b0C'}},y2:{display:false,min:0,max:1}},plugins:{legend:{display:true}}});}";
    page += "window.addEventListener('load',async()=>{const d=await load();document.getElementById('btn24h').onclick=()=>draw(d,24);document.getElementById('btn7d').onclick=()=>draw(d,168);draw(d,24);});";
    page += "</script></body></html>";
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
