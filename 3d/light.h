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
        light();
        light(const light&) = default; // �R�s�[
        light(light&&) = default; // ���[�u

        // �f�X�g���N�^
        virtual ~light() = default;

        light& operator =(const light&) = default; // �R�s�[
        light& operator =(light&&) = default; // ���[�u

        bool initialize();
        bool set_direction(const math::vector4& vector);

        const std::shared_ptr<math::vector4> get_direction() const { return direction; }

    private:
        std::shared_ptr<math::vector4> direction;
    };

} // r3d
