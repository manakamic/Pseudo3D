#pragma once

#include <cstdint>
#include <array>

namespace image {

    enum class color_kind {
        r = 0,
        g,
        b,
        a,
        max
    };

    constexpr uint32_t color_r   = static_cast<uint32_t>(color_kind::r);
    constexpr uint32_t color_g   = static_cast<uint32_t>(color_kind::g);
    constexpr uint32_t color_b   = static_cast<uint32_t>(color_kind::b);
    constexpr uint32_t color_a   = static_cast<uint32_t>(color_kind::a);
    constexpr uint32_t color_max = static_cast<uint32_t>(color_kind::max);

    constexpr uint8_t byte_zero = 0x00;
    constexpr uint8_t byte_max = 0xff;
    constexpr double double_byte_max = 255.0;

    class color {
    public:
        color();
        color(const uint8_t r, const uint8_t g, const uint8_t b);
        color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
        color(const color&) = default; // コピー
        color(color&&) = default; // ムーブ

        // デストラクタ
        virtual ~color() = default;

        uint8_t get_r() const { return rgba[color_r]; }
        uint8_t get_g() const { return rgba[color_g]; }
        uint8_t get_b() const { return rgba[color_b]; }
        uint8_t get_a() const { return rgba[color_a]; }

        color& operator =(const color&) = default; // コピー
        color& operator =(color&&) = default; // ムーブ

        const double operator [](const color_kind kind) const;
        const double operator [](const uint32_t kind) const;

    private:
        std::array<uint8_t, color_max> rgba;
    };

} // image
