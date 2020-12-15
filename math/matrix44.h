#pragma once

#include <cstdint>
#include <array>
#include <tuple>

namespace math {
    class vector3;

    constexpr uint32_t row_max = 4;
    constexpr uint32_t column_max = 4;

    using matrix_array = std::array<std::array<double, column_max>, row_max>;

    class matrix44 {
    public:
        // コンストラクタ
        matrix44();
        matrix44(const matrix_array values);
        matrix44(const matrix44&) = default; // コピー
        matrix44(matrix44&&) = default; // ムーブ

        // デストラクタ
        virtual ~matrix44() = default;

        matrix44& operator =(const matrix44&) = default; // コピー
        matrix44& operator =(matrix44&&) = default; // ムーブ

        const matrix44 operator *(const matrix44 rhs) const;
        const matrix44 operator +(const vector3 rhs) const;

        const double get_value(const uint32_t row, const uint32_t column) const { return row_column[row][column]; }
        const double set_value(const uint32_t row, const uint32_t column, const double value) { row_column[row][column] = value; }

        void zero(matrix_array& target) const;
        void unit();

        void perspective(const double fov_y, const double aspect, const double near_z, const double far_z);
        void viewport(const double width, const double height/*, const double min_z, const double max_z*/);

        void transfer(const double x, const double y, const double z, bool make);
        void scale(const double x, const double y, const double z, bool make);
        void rotate_x(const double degree, bool make);
        void rotate_y(const double degree, bool make);
        void rotate_z(const double degree, bool make);

    private:
        std::tuple<double, double> get_sin_cos(const double degree) const;

        matrix_array row_column;
    };

}
