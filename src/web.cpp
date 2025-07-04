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
    ChauffageManager::Schedule sch = chauffage->getSchedule();
    page += "<!DOCTYPE html><html lang='fr'><head><meta charset='utf-8'>";
    page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    page += "<title>Chauffe Piscine</title>";
    page += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
    page += "<script src='https://cdn.jsdelivr.net/npm/chartjs-adapter-date-fns'></script>";
    page += "<style>body{background:#fff;color:#000;font-family:Arial,Helvetica,sans-serif;margin:0;padding:1rem;}";
    page += "fieldset{border:1px solid #000;margin-bottom:.5rem;padding:.5rem;}label{display:block;margin:.25rem 0;}";
    page += "#chart{width:100%;max-width:600px;height:300px;}button{margin:.25rem;}";
    page += "</style></head><body>";
    page += "<h1>Chauffe Piscine</h1>";
    page += "<p>Eau: " + Utils::formatFloat(sensors->getWaterTemp()) + "&deg;C - Air: " + Utils::formatFloat(sensors->getAirTemp()) + "&deg;C</p>";
    page += "<form id='frm' action='/set' method='get'>";
    page += "<fieldset><legend>Mode</legend>";
    page += "<label><input type='radio' name='mode' value='MANUEL'";
    if (chauffage->getMode() == ChauffageManager::MANUEL) page += " checked";
    page += ">Manuel</label>";
    page += "<label><input type='radio' name='mode' value='AUTO'";
    if (chauffage->getMode() == ChauffageManager::AUTO) page += " checked";
    page += ">Auto</label></fieldset>";
    page += "<div id='manualFields'><label><input type='radio' name='manuelEtat' value='ON'";
    if (chauffage->isOn()) page += " checked";
    page += ">Forcer ON</label><label><input type='radio' name='manuelEtat' value='OFF'";
    if (!chauffage->isOn()) page += " checked";
    page += ">Forcer OFF</label></div>";
    page += "<div id='autoFields'><label>Température cible:<input type='number' name='tempCible' min='10' max='35' step='1' value='" + String((int)round(chauffage->getTargetTemp())) + "'></label></div>";
    page += "<div id='scheduleOptions'><label><input type='checkbox' id='scheduleActive' name='plageActive'";
    if (sch.enabled) page += " checked";
    page += ">Activer plage horaire</label><div id='scheduleTimes'><label>Début:<input type='time' name='heureDebut' value='" + Utils::formatTime(sch.startHour, sch.startMin) + "'></label><label>Fin:<input type='time' name='heureFin' value='" + Utils::formatTime(sch.endHour, sch.endMin) + "'></label></div></div>";
    page += "<button type='submit'>Appliquer</button></form>";
    page += "<div><button id='v24'>Vue 24h</button><button id='v7'>Vue 7j</button></div><canvas id='chart'></canvas>";
    page += "<script>function q(s){return document.querySelector(s);}function update(){var mode=q('input[name=mode]:checked').value;var manual=q('input[name=\\"manuelEtat\\"]:checked');var on=manual&&manual.value==='ON';q('#autoFields').style.display=mode==='AUTO'?'block':'none';q('#manualFields').style.display=mode==='MANUEL'?'block':'none';q('#scheduleOptions').style.display=(mode==='AUTO'||on)?'block':'none';toggleSchedule();}function toggleSchedule(){q('#scheduleTimes').style.display=q('#scheduleActive').checked?'block':'none';}document.querySelectorAll('input[name=mode]').forEach(e=>e.addEventListener('change',update));document.querySelectorAll('input[name=manuelEtat]').forEach(e=>e.addEventListener('change',update));q('#scheduleActive').addEventListener('change',toggleSchedule);update();let history=[],heating=[],chart;async function load(){const h=await fetch('/history.csv');const ht=await h.text();const c=await fetch('/chauffage.csv');const ct=await c.text();history=ht.trim().split('\n').slice(1).map(l=>{const p=l.split(',');return{t:new Date(p[0]),w:+p[1],a:+p[2]}});heating=ct.trim().split('\n').slice(1).map(l=>{const p=l.split(',');return{t:new Date(p[0]),on:p[1].trim()==='ON'}});const ctx=q('#chart').getContext('2d');chart=new Chart(ctx,{type:'line',data:{datasets:[{label:'Eau',borderColor:'blue',data:[],fill:false},{label:'Air',borderColor:'gray',data:[],fill:false},{label:'Chauffage',borderColor:'red',backgroundColor:'rgba(255,0,0,0.3)',data:[],stepped:true,fill:true,yAxisID:'y2'}]},options:{responsive:true,scales:{x:{type:'time'},y:{title:{display:true,text:'°C'}},y2:{display:false,min:0,max:1}}}});draw('24h');}function draw(r){const now=new Date();const from=new Date(now-(r==='24h'?86400000:604800000));const h=history.filter(e=>e.t>=from);const s=heating.filter(e=>e.t>=from);chart.data.datasets[0].data=h.map(e=>({x:e.t,y:e.w}));chart.data.datasets[1].data=h.map(e=>({x:e.t,y:e.a}));let d=[],state=0;d.push({x:from,y:0});for(const p of s){d.push({x:p.t,y:state});state=p.on?1:0;d.push({x:p.t,y:state});}d.push({x:now,y:state});chart.data.datasets[2].data=d;chart.update();}q('#v24').onclick=()=>draw('24h');q('#v7').onclick=()=>draw('7d');load();</script></body></html>";
    server.send(200, "text/html; charset=utf-8", page);
}

void WebManager::handleSet() {
    if (server.hasArg("mode")) {
        String m = server.arg("mode");
        if (m == "MANUEL") chauffage->setMode(ChauffageManager::MANUEL);
        else chauffage->setMode(ChauffageManager::AUTO);
    }
    if (server.hasArg("tempCible")) {
        chauffage->setTargetTemp(server.arg("tempCible").toFloat());
    }
    ChauffageManager::Schedule sch = chauffage->getSchedule();
    sch.enabled = server.hasArg("plageActive");
    if (server.hasArg("heureDebut")) {
        sscanf(server.arg("heureDebut").c_str(), "%2hhu:%2hhu", &sch.startHour, &sch.startMin);
    }
    if (server.hasArg("heureFin")) {
        sscanf(server.arg("heureFin").c_str(), "%2hhu:%2hhu", &sch.endHour, &sch.endMin);
    }
    chauffage->setSchedule(sch);
    if (server.hasArg("manuelEtat")) {
        if (server.arg("manuelEtat") == "ON") chauffage->manualOn();
        else chauffage->manualOff();
    }
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
