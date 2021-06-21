#include <cmath>
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

    void color::set(const color& color) {
        rgba[color_r] = color.get_r();
        rgba[color_g] = color.get_g();
        rgba[color_b] = color.get_b();
        rgba[color_a] = color.get_a();
    }

    const color color::operator *(const double rhs) const {
        auto dr = static_cast<double>(rgba[color_r]) * rhs;
        auto dg = static_cast<double>(rgba[color_g]) * rhs;
        auto db = static_cast<double>(rgba[color_b]) * rhs;
        auto da = static_cast<double>(rgba[color_a]) * rhs;

        auto r = static_cast<uint8_t>(std::round(dr));
        auto g = static_cast<uint8_t>(std::round(dg));
        auto b = static_cast<uint8_t>(std::round(db));
        auto a = static_cast<uint8_t>(std::round(da));

        return  color(r, g, b, a);
    }

    const color color::operator +(const color& rhs) const {
        auto r = static_cast<double>(rgba[color_r]) + static_cast<double>(rhs.get_r());
        auto g = static_cast<double>(rgba[color_g]) + static_cast<double>(rhs.get_g());
        auto b = static_cast<double>(rgba[color_b]) + static_cast<double>(rhs.get_b());
        auto a = static_cast<double>(rgba[color_a]) + static_cast<double>(rhs.get_a());

        if (r > double_byte_max) {
            r = double_byte_max;
        }

        if (g > double_byte_max) {
            g = double_byte_max;
        }

        if (b > double_byte_max) {
            b = double_byte_max;
        }

        if (a > double_byte_max) {
            a = double_byte_max;
        }

        return  color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), static_cast<uint8_t>(a));
    }

    const double color::operator [](const color_kind kind) const {
        return static_cast<double>(rgba[static_cast<uint32_t>(kind)]) / double_byte_max;
    }

    const double color::operator [](const uint32_t kind) const {
        return static_cast<double>(rgba[kind]) / double_byte_max;
    }

}
