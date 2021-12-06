#include <algorithm>
#include <vector>

std::vector<int> getHSL(const int red, const int green, const int blue) {
    float r = (red / 255.0f);
    float g = (green / 255.0f);
    float b = (blue / 255.0f);

    float min = std::min({r, g, b});
    float max = std::max({r, g, b});
    float delta = max - min;

    const double l = (max + min) / 2;

    if (delta == 0) {
        const double h = 0;
        const double s = 0;
        return {(int)h, (int)s, (int)l};
    } else {
        const double s = (l <= 0.5) ? (delta / (max + min)) : (delta / (2 - max - min));

        double h;

        if (r == max) {
            h = ((g - b) / 6) / delta;
        } else if (g == max) {
            h = (1.0f / 3) + ((b - r) / 6) / delta;
        } else {
            h = (2.0f / 3) + ((r - g) / 6) / delta;
        }

        if (h < 0) h += 1;
        if (h > 1) h -= 1;

        h *= 360;
        return {(int)h, (int)s, (int)l};
    }
}
