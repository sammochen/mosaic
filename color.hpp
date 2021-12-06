#ifndef COLOR_HPP
#define COLOR_HPP

#include <stdint.h>
#include <stdlib.h>

struct Color {
    uint8_t r, g, b;

    int dist(const Color &color) const {
        int dr = abs(r - (int)color.r);
        int dg = abs(g - (int)color.g);
        int db = abs(b - (int)color.b);
        return dr * dr + dg * dg + db * db;
    }

    bool operator==(const Color &color) const {
        return r == color.r && g == color.g && b == color.b;
    }

    bool operator!=(const Color &color) const { return !operator==(color); }
};

#endif
