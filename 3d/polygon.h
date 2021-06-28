#pragma once

#include <cstdint>
#include <memory>
#include <array>

namespace math {
    class vector4;
    class matrix44;
}

#if defined(_USE_LIGHTING)
namespace r3d {
    class camera;
}
#endif

namespace r3d {
    class vertex;

    constexpr uint32_t polygon_vertices_num = 4;

    class polygon {
    public:
        // �R���X�g���N�^
        polygon();
        polygon(const polygon&) = default; // �R�s�[
        polygon(polygon&&) = default; // ���[�u

        // �f�X�g���N�^
        virtual ~polygon() = default;

        polygon& operator =(const polygon&) = default; // �R�s�[
        polygon& operator =(polygon&&) = default; // ���[�u

        bool initialize();
        virtual bool transform(const math::matrix44& matrix, const bool transform);

        void set_vertices(const std::shared_ptr<r3d::vertex>& v0, const std::shared_ptr<r3d::vertex>& v1,
                          const std::shared_ptr<r3d::vertex>& v2, const std::shared_ptr<r3d::vertex>& v3);

        const std::shared_ptr<r3d::vertex> get_vertex(const uint32_t number) const { return vertices[number]; }
        bool get_center(math::vector4& center, const bool transform) const;

#if defined(_USE_LIGHTING)
        void set_camera(const std::shared_ptr<r3d::camera>& camera) { camera_ptr = camera; }
#endif

    protected:
        std::array<std::shared_ptr<r3d::vertex>, polygon_vertices_num> vertices;
        std::array<std::shared_ptr<r3d::vertex>, polygon_vertices_num> transform_vertices;

#if defined(_USE_LIGHTING)
        std::shared_ptr<r3d::camera> camera_ptr;
#endif
    };

} // r3d
