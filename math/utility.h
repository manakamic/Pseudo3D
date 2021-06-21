#pragma once
#include <tuple>
#if defined(_DEBUG_3D)
#include <memory>
#include <vector>
#endif

namespace math {

    class vector4;

    using plane  = std::tuple<vector4/*point*/, vector4/*normal*/>;
    using collision = std::tuple<bool, vector4>;

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

        // 矩形と線分の当たり判定
        static collision collision_polygon_line(const vector4& polygon_point0, const vector4& polygon_point1,
                                                const vector4& polygon_point2, const vector4& polygon_point3,
                                                const vector4& line_start, const vector4& line_end);

        // 面の法線を求める
        static vector4 get_normal(const vector4& point0, const vector4& point1, const vector4& point2);

        // 三角形と点の内外判定(point は必ず三角形と同平面である事)
        static bool inside_triangle_point(const vector4& triangle_point0, const vector4& triangle_point1, const vector4& triangle_point2, const vector4& point);

        // 平面と線分の辺り判定
        static bool collision_plane_line(const plane& p, const vector4& line_start, const vector4& line_end, collision& result);

#if defined(_DEBUG_3D)
        static std::shared_ptr<vector4> collision_point;
#endif

    private:
        utility() = default;
    };
}
