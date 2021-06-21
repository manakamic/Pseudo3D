#pragma once

#include <memory>

namespace image {
    class color;
}

namespace math {
    class vector4;
}

namespace r3d {

    class light {
    public:
        // コンストラクタ
        light() = default;
        light(const light&) = default; // コピー
        light(light&&) = default; // ムーブ

        // デストラクタ
        virtual ~light() = default;

        light& operator =(const light&) = default; // コピー
        light& operator =(light&&) = default; // ムーブ

    private:
        std::unique_ptr<math::vector4> direction;
        std::unique_ptr<image::color> diffuse;
        std::unique_ptr<image::color> speculer;
        std::unique_ptr<image::color> ambient;
    };

} // r3d
