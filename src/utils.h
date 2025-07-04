#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// Miscellaneous helper utilities
class Utils {
public:
    static String formatFloat(float value, uint8_t decimals = 1);
    static String timestamp();
    static String formatTime(uint8_t hour, uint8_t minute);
};

#endif // UTILS_H
