#include <random>
#include <cmath>
#include "vector4.h"
#include "utility.h"

namespace {
    std::random_device seed_gen;
    std::mt19937_64 random(seed_gen());
}

namespace math {

#if defined(_DEBUG_3D)
    std::shared_ptr<vector4> utility::collision_point = nullptr;
#endif

    int utility::get_random(const int min, const int max) {
        std::uniform_int_distribution<int>  distr(min, max);

        return distr(random);
    }

    // ��`�Ɛ����̓����蔻��
    collision utility::collision_polygon_line(const vector4& polygon_point0, const vector4& polygon_point1,
                                              const vector4& polygon_point2, const vector4& polygon_point3,
                                              const vector4& line_start, const vector4& line_end) {
        // ���ʂ̏����Z�b�g
        const auto normal = get_normal(polygon_point0, polygon_point1, polygon_point2);
        const auto plane  = std::make_tuple(polygon_point0, normal);
        auto result = std::make_tuple(false, vector4());

        // ���ʂƐ���������邩
        if (!collision_plane_line(plane, line_start, line_end, result)) {
            return result;
        }

        // ���ʂƂ̌�_
        const vector4 point = std::get<1>(result);

#if defined(_DEBUG_3D)
        collision_point.reset(new vector4(point));
#endif

        // ��`���O�p�`�ɕ����Ĕ���
        if (inside_triangle_point(polygon_point0, polygon_point1, polygon_point2, point)) {
            std::get<0>(result) = true;
            std::get<1>(result) = point;

            return result;
        }

        if (inside_triangle_point(polygon_point1, polygon_point3, polygon_point2, point)) {
            std::get<0>(result) = true;
            std::get<1>(result) = point;

            return result;
        }

        std::get<0>(result) = false;

        return result;
    }

    // �ʂ̖@�������߂�
    vector4 utility::get_normal(const vector4& point0, const vector4& point1, const vector4& point2) {
        // �ʂ̖@�������߂�
        const auto v0 = point1 - point0;
        const auto v1 = point2 - point0;
        auto cross = v0.cross(v1);

        return cross.normalize();
    }

    // �O�p�`�Ɠ_�̓��O����(point �͕K���O�p�`�Ɠ����ʂł��鎖)
    bool utility::inside_triangle_point(const vector4& triangle_point0, const vector4& triangle_point1, const vector4& triangle_point2, const vector4& point) {
        const auto v01 = triangle_point1 - triangle_point0;
        const auto v12 = triangle_point2 - triangle_point1;
        const auto v21 = triangle_point0 - triangle_point2;
        const auto v0p = point - triangle_point0;
        const auto v1p = point - triangle_point1;
        const auto v2p = point - triangle_point2;

        auto c0 = v01.cross(v1p);
        auto c1 = v12.cross(v2p);
        auto c2 = v21.cross(v0p);

        c0.normalized();
        c1.normalized();
        c2.normalized();

        const auto dot01 = c0.dot(c1);
        const auto dot02 = c0.dot(c2);

        return (dot01 > 0.0) && (dot02 > 0.0);
    }

    // ���ʂƐ����̓����蔻��
    bool utility::collision_plane_line(const plane& p, const vector4& line_start, const vector4& line_end, collision& result) {
        const vector4 plane_point  = std::get<0>(p);
        const vector4 plane_normal = std::get<1>(p);
        auto point_to_start = line_start - plane_point;
        auto point_to_end   = line_end   - plane_point;

        point_to_start.normalized();
        point_to_end.normalized();

        const auto dot_p_s = plane_normal.dot(point_to_start);
        const auto dot_p_e = plane_normal.dot(point_to_end);
        const auto pattern_a = (dot_p_s >= 0.0) && (dot_p_e <= 0.0);
        const auto pattern_b = (dot_p_s <= 0.0) && (dot_p_e >= 0.0);
        const auto ret = pattern_a || pattern_b;

        if (ret) {
            // ���ʂƐ����̌�_
            const auto line = line_end - line_start;
            const auto ratio = std::abs(dot_p_s) / std::abs(dot_p_s) + std::abs(dot_p_e);
            const auto point = line_start + (line * ratio);

            std::get<1>(result) = point;
        }

        return ret;
    }
}
