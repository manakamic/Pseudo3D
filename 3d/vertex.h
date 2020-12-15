#pragma once

#include <cstdint>
#include <memory>
//#include <array>

/*
namespace image {
    class color;
}
*/

namespace math {
    class vector3;
}

namespace r3d {
    /*
    enum class uv_kind {
        u = 0,
        v,
        max
    };

    constexpr uint32_t uv_max = static_cast<uint32_t>(uv_kind::max);
    */

    class vertex {
    public:
        // �R���X�g���N�^
        vertex();
        vertex(const vertex&) = default; // �R�s�[
        vertex(vertex&&) = default; // ���[�u

        // �f�X�g���N�^
        virtual ~vertex() = default;

        vertex& operator =(const vertex&) = default; // �R�s�[
        vertex& operator =(vertex&&) = default; // ���[�u

        bool initialize();
        bool set_position(const math::vector3& vector);
        const std::shared_ptr<math::vector3> get_position() const { return position; }

    private:
        std::shared_ptr<math::vector3> position;
        /*
        std::unique_ptr<math::vector3> normal;

        std::array<double, uv_max> uv;

        std::unique_ptr<image::color> diffuse;
        std::unique_ptr<image::color> speculer;
        double speculer_power;
        */
    };

} // r3d
