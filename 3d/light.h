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
        light();
        light(const light&) = default; // コピー
        light(light&&) = default; // ムーブ

        // デストラクタ
        virtual ~light() = default;

        light& operator =(const light&) = default; // コピー
        light& operator =(light&&) = default; // ムーブ

        bool initialize();
        bool set_direction(const math::vector4& vector);

        const std::shared_ptr<math::vector4> get_direction() const { return direction; }

    private:
        std::shared_ptr<math::vector4> direction;
    };

} // r3d
