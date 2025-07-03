#include "utils.h"

String Utils::formatFloat(float value, uint8_t decimals) {
    char buf[16];
    dtostrf(value, 0, decimals, buf);
    return String(buf);
}

