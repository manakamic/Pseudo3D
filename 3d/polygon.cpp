#include "vector4.h"
#include "matrix44.h"
#include "vertex.h"
#include "polygon.h"

namespace r3d {

    polygon::polygon() {
        vertices.fill(nullptr);
        transform_vertices.fill(nullptr);
#if defined(_USE_LIGHTING)
        camera_ptr = nullptr;
#endif
    }

    bool polygon::initialize() {
        for (auto i = 0; i < polygon_vertices_num; ++i) {
            std::shared_ptr<r3d::vertex> vertex(new r3d::vertex);
            std::shared_ptr<r3d::vertex> transform_vertex(new r3d::vertex);

            if (!vertex->initialize() || !transform_vertex->initialize()) {
                return false;
            }

            vertices[i] = vertex;
            transform_vertices[i] = transform_vertex;
        }

        return true;
    }

    bool polygon::transform(const math::matrix44& matrix, const bool transform) {
        for (auto i = 0; i < polygon_vertices_num; ++i) {
            std::shared_ptr<r3d::vertex> src = transform ? transform_vertices[i] : vertices[i];
            std::shared_ptr<r3d::vertex> dst = transform_vertices[i];

            if (src == nullptr || dst == nullptr) {
                return false;
            }

            auto src_pos = src->get_position();

            if (src_pos == nullptr) {
                return false;
            }

            auto trans_pos = src_pos->mult_with_w(matrix);

            dst->set_position(trans_pos);
#if defined(_USE_RASTERIZE)
            dst->set_uv(src->get_uv());
#endif
#if defined(_USE_LIGHTING)
            // TODO : 頂点と法線の処理
            dst->set_world_position(*src_pos);
            dst->set_normal(*src->get_normal());
#if defined(_USE_NORMAL_MAP)
            dst->set_tangent(*src->get_tangent());
            dst->set_binormal(*src->get_binormal());
#endif
            dst->set_diffuse(*src->get_diffuse());
            dst->set_speculer(*src->get_speculer(), src->get_speculer_power());
#endif
        }

        return true;
    }

    void polygon::set_vertices(const std::shared_ptr<r3d::vertex>& v0, const std::shared_ptr<r3d::vertex>& v1,
                               const std::shared_ptr<r3d::vertex>& v2, const std::shared_ptr<r3d::vertex>& v3) {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
        vertices[3] = v3;
    }

    bool polygon::get_center(math::vector4& center, const bool transform) const {
        std::array<std::shared_ptr<r3d::vertex>, polygon_vertices_num> v = transform ? transform_vertices : vertices;

        if (v[0] == nullptr || v[1] == nullptr || v[2] == nullptr || v[3] == nullptr) {
            return false;
        }

        center = math::vector4();
        auto  count = 0.0;

        for (auto i = 0; i < polygon_vertices_num; ++i) {
            auto pos = v[i]->get_position();

            if (pos == nullptr) {
                return false;
            }

            center = center + *pos;
            count += 1.0;
        }

        center = center / count;

        return true;
    }

}
