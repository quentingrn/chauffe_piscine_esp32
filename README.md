# Chauffe Piscine ESP32 🏊‍♂️🔥

Projet domotique open source basé sur ESP32 pour piloter automatiquement le chauffage d'une piscine en fonction de la température de l’eau et d’un horaire cible.

---

## ⚙️ Fonctionnalités

- Lecture température eau + air (DS18B20)
- Estimation automatique du moment de chauffe
- Contrôle d’un servo-moteur interrupteur intex slider
- Mode automatique ou manuel via interface web
- Interface web locale avec :
  - Lecture des températures
  - Historique graphique (CSV + Chart.js)
  - Commandes manuelles
- Horloge temps réel via NTP
- Stockage SPIFFS pour les historiques

---

## 📡 Matériel requis

- ESP32 (DevKit)
- 2 sondes DS18B20
- 2 résistance 4.7kΩ (pull-up)
- 1 servo-moteur (SG90 ou équivalent)
- Accès Wi-Fi

---

## 🔧 Configuration

Les paramètres modifiables (Wi-Fi, broches GPIO, positions du servo, etc.) se trouvent dans `src/config.h`.
Avant de compiler, éditez ce fichier pour y renseigner votre SSID, mot de passe
et éventuellement la température ou l'heure cible.
Vous pouvez également ajuster `SERVO_POS_ON` et `SERVO_POS_OFF` pour adapter les angles d'activation du chauffage.

Si vous utilisez VS Code avec PlatformIO, vous pouvez spécifier le port série à
utiliser dans `platformio.ini` via les directives `upload_port` et
`monitor_port`. Par exemple :

```ini
upload_port = /dev/cu.usbserial-0001
monitor_port = /dev/cu.usbserial-0001
```

---

## 🧱 Arborescence du projet
chauffe_piscine_esp32/
├── src/
│ ├── main.cpp
│ ├── config.h
│ ├── sensors.cpp / .h
│ ├── chauffage.cpp / .h
│ ├── web.cpp / .h
│ └── utils.cpp / .h
├── data/
│ ├── chauffage.csv # journalisation ON/OFF
│ └── history.csv # températures eau/air
├── .gitignore
└── README.md
