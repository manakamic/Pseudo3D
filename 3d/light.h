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
        // �R���X�g���N�^
        light() = default;
        light(const light&) = default; // �R�s�[
        light(light&&) = default; // ���[�u

        // �f�X�g���N�^
        virtual ~light() = default;

        light& operator =(const light&) = default; // �R�s�[
        light& operator =(light&&) = default; // ���[�u

    private:
        std::unique_ptr<math::vector4> direction;
        std::unique_ptr<image::color> diffuse;
        std::unique_ptr<image::color> speculer;
        std::unique_ptr<image::color> ambient;
    };

} // r3d
