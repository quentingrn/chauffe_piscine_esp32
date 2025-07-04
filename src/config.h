#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi credentials
static const char* WIFI_SSID = "YOUR_SSID";
static const char* WIFI_PASS = "YOUR_PASSWORD";

// Target temperature when using manual set (float)
static constexpr float TARGET_TEMPERATURE = 33.0f;

// Target time for heating start (HH:MM)
static const char* TARGET_TIME = "18:00";

// GPIO pin numbers
static constexpr uint8_t SERVO_PIN = 14;
static constexpr uint8_t ONE_WIRE_PIN = 4;

// Optional API key for future features
static const char* API_KEY = "";

#endif // CONFIG_H
