#pragma once
#include <tuple>
#if _DEBUG_3D
#include <memory>
#include <vector>
#include <array>
#endif

namespace math {

    class vector3;

    using plane  = std::tuple<vector3/*point*/, vector3/*normal*/>;
    using collision = std::tuple<bool, vector3>;

    class utility {
    public:
        static constexpr double PI = 3.14159265358979;
        static constexpr double DEGREES_180 = 180.0;

        static double degree_to_radian(double degree) {
            return degree * PI / DEGREES_180;
        }

        static double radian_to_degree(double radion) {
            return radion * DEGREES_180 / PI;
        }

        static int get_random(const int min, const int max);
        static bool get_random() {
            return get_random(1, 2) == 1 ? true : false;
        }

        // ��`�Ɛ����̓����蔻��
        static collision collision_polygon_line(const vector3 polygon_point0, const vector3 polygon_point1,
                                                const vector3 polygon_point2, const vector3 polygon_point3,
                                                const vector3 line_start, const vector3 line_end);

        // �ʂ̖@�������߂�
        static vector3 get_normal(const vector3& point0, const vector3& point1, const vector3& point2);

        // �O�p�`�Ɠ_�̓��O����(point �͕K���O�p�`�Ɠ����ʂł��鎖)
        static bool inside_triangle_point(const vector3 triangle_point0, const vector3 triangle_point1, const vector3 triangle_point2, const vector3 point);

        // ���ʂƐ����̕ӂ蔻��
        static bool collision_plane_line(const plane p, const vector3 line_start, const vector3 line_end, collision& result);

#if _DEBUG_3D
        static std::shared_ptr<vector3> collision_point;
#endif

    private:
        utility() = default;
    };
}
