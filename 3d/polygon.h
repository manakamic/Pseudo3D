#pragma once

#include <cstdint>
#include <memory>
#include <array>

namespace math {
    class vector3;
    class matrix44;
}

namespace r3d {
    class vertex;

    constexpr uint32_t polygon_vertices_num = 4;

    class polygon {
    public:
        // コンストラクタ
        polygon();
        polygon(const polygon&) = default; // コピー
        polygon(polygon&&) = default; // ムーブ

        // デストラクタ
        virtual ~polygon() = default;

        polygon& operator =(const polygon&) = default; // コピー
        polygon& operator =(polygon&&) = default; // ムーブ

        bool initialize();
        virtual bool transform(const math::matrix44& matrix, const bool transform);

        void set_vertices(const std::shared_ptr<r3d::vertex>& v0, const std::shared_ptr<r3d::vertex>& v1,
                          const std::shared_ptr<r3d::vertex>& v2, const std::shared_ptr<r3d::vertex>& v3);

        const std::shared_ptr<r3d::vertex> get_vertex(const uint32_t number) const { return vertices[number]; }
        bool get_center(math::vector3& center, const bool transform) const;

    protected:
        std::array<std::shared_ptr<r3d::vertex>, polygon_vertices_num> vertices;
        std::array<std::shared_ptr<r3d::vertex>, polygon_vertices_num> transform_vertices;
    };

} // r3d
