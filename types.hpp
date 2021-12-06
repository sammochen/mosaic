// All custom type declarations

#include <stdint.h>
#include <stdlib.h>

#include <vector>

struct Color {
    uint8_t r, g, b;

    int dist(const Color &color) const {
        int dr = abs(r - (int)color.r);
        int dg = abs(g - (int)color.g);
        int db = abs(b - (int)color.b);
        return dr * dr + dg * dg + db * db;
    }
};

using ImageArray = std::vector<std::vector<Color>>;
using ImageData = uint8_t *;
