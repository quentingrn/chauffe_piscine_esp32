#include "utils.h"
#include <time.h>

String Utils::formatFloat(float value, uint8_t decimals) {
    char buf[16];
    dtostrf(value, 0, decimals, buf);
    return String(buf);
}

String Utils::timestamp() {
    time_t now;
    time(&now);
    struct tm *tm_info = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
    return String(buf);
}

String Utils::formatTime(uint8_t hour, uint8_t minute) {
    char buf[6];
    snprintf(buf, sizeof(buf), "%02u:%02u", hour, minute);
    return String(buf);
}

