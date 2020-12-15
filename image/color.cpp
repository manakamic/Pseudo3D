#include "color.h"

namespace image {

    color::color() {
        rgba[color_r] = byte_max;
        rgba[color_g] = byte_max;
        rgba[color_b] = byte_max;
        rgba[color_a] = byte_zero;
    }

    color::color(const uint8_t r, const uint8_t g, const uint8_t b) {
        rgba[color_r] = r;
        rgba[color_g] = g;
        rgba[color_b] = b;
        rgba[color_a] = byte_zero;
    }

    color::color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
        rgba[color_r] = r;
        rgba[color_g] = g;
        rgba[color_b] = b;
        rgba[color_a] = a;
    }

    const double color::operator [](const color_kind kind) const {
        return static_cast<double>(rgba[static_cast<uint32_t>(kind)]) / double_byte_max;
    }

    const double color::operator [](const uint32_t kind) const {
        return static_cast<double>(rgba[kind]) / double_byte_max;
    }

}
