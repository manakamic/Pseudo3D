#pragma once

#include <memory>
#include <array>

/*
namespace image {
    class color;
}
*/

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

    private:
        std::shared_ptr<math::vector4> position;
        //std::unique_ptr<math::vector4> normal;

        std::array<double, uv_max> uv;

        /*
        std::unique_ptr<image::color> diffuse;
        std::unique_ptr<image::color> speculer;
        double speculer_power;
        */
    };

} // r3d
