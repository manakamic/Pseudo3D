#pragma once

#include <memory>
#include <array>

#if defined(_USE_LIGHTING)
namespace image {
    class color;
}
#endif

namespace math {
    class vector4;
}

namespace r3d {

    constexpr auto uv_max = 2;

    class vertex {
    public:
        // コンストラクタ
        vertex();
        vertex(const vertex&) = default; // コピー
        vertex(vertex&&) = default; // ムーブ

        // デストラクタ
        virtual ~vertex() = default;

        vertex& operator =(const vertex&) = default; // コピー
        vertex& operator =(vertex&&) = default; // ムーブ

        bool initialize();
        bool set_position(const math::vector4& vector);
        const std::shared_ptr<math::vector4> get_position() const { return position; }

        void set_uv(const double u, const double v) { uv[0] = u; uv[1] = v; }
        void set_uv(const std::array<double, uv_max>& uv) { this->uv[0] = uv[0]; this->uv[1] = uv[1]; }
        const std::array<double, uv_max> get_uv() const { return uv; }

#if defined(_USE_LIGHTING)
        bool set_normal(const math::vector4& vector);
        const std::shared_ptr<math::vector4> get_normal() const { return normal; }

        bool set_diffuse(const image::color& color);
        const std::shared_ptr<image::color> get_diffuse() const { return diffuse; }

        bool set_speculer(const image::color& color, double power);
        const std::shared_ptr<image::color> get_speculer() const { return speculer; }
        double get_speculer_power() const { return speculer_power; }
#endif

    private:
        std::shared_ptr<math::vector4> position;

        std::array<double, uv_max> uv;

#if defined(_USE_LIGHTING)
        std::shared_ptr<math::vector4> normal;
        std::shared_ptr<image::color> diffuse;
        std::shared_ptr<image::color> speculer;
        double speculer_power;
#endif
    };

} // r3d
