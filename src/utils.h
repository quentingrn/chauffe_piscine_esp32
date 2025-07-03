#ifndef UTILS_H
#define UTILS_H

// Miscellaneous helper utilities
class Utils {
public:
    static String formatFloat(float value, uint8_t decimals = 1);
    static String timestamp();
};

#endif // UTILS_H
