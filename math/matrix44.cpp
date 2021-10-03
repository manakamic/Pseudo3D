#include <cmath>
#include "utility.h"
#include "matrix44.h"
#include "vector4.h"

namespace math {

    matrix44::matrix44() {
        unit();
    }

    matrix44::matrix44(const matrix_array values) {
        this->row_column = values;
    }

    void matrix44::zero(matrix_array& target) const {
        for (auto i = 0; i < row_max; ++i) {
            target[i].fill(0.0);
        }
    }

    void matrix44::unit() {
        zero(row_column);

        for (auto i = 0; i < row_max; ++i) {
            row_column[i][i] = 1.0;
        }
    }

    void matrix44::perspective(const double fov_y, const double aspect, const double near_z, const double far_z) {
        auto cot = 1.0 / tan(fov_y * 0.5);
        auto range = far_z - near_z;
        auto temp = far_z / range;

        zero(row_column);

        row_column[0][0] = cot * aspect;
        row_column[1][1] = cot;
        row_column[2][2] = temp;
        row_column[2][3] = 1.0f;
        row_column[3][2] = -near_z * temp;
    }

    void matrix44::viewport(const double width, const double height/*, const double min_z, const double max_z*/) {
        auto w = width  * 0.5;
        auto h = height * 0.5;

        unit();

        row_column[0][0] =  w;
        row_column[1][1] = -h;
        row_column[3][0] =  w;
        row_column[3][1] =  h;

        // ZƒNƒŠƒbƒv
#if 0
        auto range = max_z / (max_z - min_z);

        row_column[2][2] = range;
        row_column[3][2] = -range * min_z;
#endif
    }

    const matrix44 matrix44::operator *(const matrix44 rhs) const {
        matrix_array result{ {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}} };

        for (auto i = 0; i < row_max; ++i) {
            for (auto j = 0; j < column_max; ++j) {
                auto m = 0.0;

                for (auto k = 0; k < 4; ++k) {
                    m += row_column[i][k] * rhs.get_value(k, j);
                }

                result[i][j] = m;
            }
        }

        return matrix44(result);
    }

    const matrix44 matrix44::operator +(const vector4 rhs) const {
        matrix_array result = row_column;

        result[3][0] += rhs.get_x();
        result[3][1] += rhs.get_y();
        result[3][2] += rhs.get_z();

        return matrix44(result);
    }

    void matrix44::transfer(const double x, const double y, const double z, bool make) {
        if (make) {
            unit();
        }

        row_column[3][0] += x;
        row_column[3][1] += y;
        row_column[3][2] += z;
    }

    void matrix44::scale(const double x, const double y, const double z, bool make) {
        if (make) {
            unit();
        }

        row_column[0][0] *= x;
        row_column[1][1] *= y;
        row_column[2][2] *= z;
    }

    void matrix44::rotate_x(const double degree, bool make) {
        auto sin_cos = get_sin_cos(degree);
        auto sin = std::get<0>(sin_cos);
        auto cos = std::get<1>(sin_cos);

        if (make) {
            unit();

            row_column[1][1] =  cos;
            row_column[1][2] =  sin;
            row_column[2][1] = -sin;
            row_column[2][2] =  cos;
        }
        else {
            matrix_array array{ {{1, 0, 0, 0}, {0, cos, sin, 0}, {0, -sin, cos, 0}, {0, 0, 0, 1}} };
            matrix44 rot_x(array);

            *this = *this * rot_x;
        }
    }

    void matrix44::rotate_y(const double degree, bool make) {
        auto sin_cos = get_sin_cos(degree);
        auto sin = std::get<0>(sin_cos);
        auto cos = std::get<1>(sin_cos);

        if (make) {
            unit();

            row_column[0][0] =  cos;
            row_column[0][2] = -sin;
            row_column[2][0] =  sin;
            row_column[2][2] =  cos;
        }
        else {
            matrix_array array{ {{cos, 0, -sin, 0}, {0, 1, 0, 0}, {sin, 0, cos, 0}, {0, 0, 0, 1}} };
            matrix44 rot_y(array);

            *this = *this * rot_y;
        }
    }

    void matrix44::rotate_z(const double degree, bool make) {
        auto sin_cos = get_sin_cos(degree);
        auto sin = std::get<0>(sin_cos);
        auto cos = std::get<1>(sin_cos);

        if (make) {
            unit();

            row_column[0][0] =  cos;
            row_column[0][1] =  sin;
            row_column[1][0] = -sin;
            row_column[1][1] =  cos;
        }
        else {
            matrix_array array{ {{cos, sin, 0, 0}, {-sin, cos, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}} };
            matrix44 rot_z(array);

            *this = *this * rot_z;
        }
    }

    const matrix44 matrix44::get_rotate() const {
        matrix_array none_transfer = row_column;

        none_transfer[3][0] = 0.0;
        none_transfer[3][1] = 0.0;
        none_transfer[3][2] = 0.0;

        return matrix44(none_transfer);
    }

    std::tuple<double, double> matrix44::get_sin_cos(const double degree) const {
        auto radian = utility::degree_to_radian(degree);

        return std::make_tuple(std::sin(radian), std::cos(radian));
    }

}
