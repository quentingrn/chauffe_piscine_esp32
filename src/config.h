#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---------------------------------------------------------------------------
// User editable parameters
// ---------------------------------------------------------------------------

// Wi-Fi credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"

// Default target temperature for AUTO mode (°C)
static constexpr float TARGET_TEMP_DEFAULT = 28.0f;

// Default mode at boot
enum StartMode { MODE_MANUEL, MODE_AUTO };
static constexpr StartMode DEFAULT_MODE = MODE_AUTO;

// GPIO pin numbers
static constexpr uint8_t PIN_SERVO = 14;      // Servo signal
static constexpr uint8_t PIN_ONEWIRE = 4;     // DS18B20 bus

// Optional API key placeholder for future use
static const char* API_KEY = "";

#endif // CONFIG_H
