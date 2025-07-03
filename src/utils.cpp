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

