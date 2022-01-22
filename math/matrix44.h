#pragma once

#include <array>
#include <tuple>

namespace math {
    class vector4;

    constexpr auto row_max = 4;
    constexpr auto column_max = 4;

    using matrix_array = std::array<std::array<double, row_max>, column_max>;

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
        const matrix44 operator +(const vector4 rhs) const;

        const double get_value(const int row, const int column) const { return row_column[row][column]; }
        const void set_value(const int row, const int column, const double value) { row_column[row][column] = value; }

        void zero(matrix_array& target) const;
        void unit();

        const double determinant() const;
        const matrix44 get_inverse() const;

        void perspective(const double fov_y, const double aspect, const double near_z, const double far_z);
        void viewport(const double width, const double height/*, const double min_z, const double max_z*/);

        void transfer(const double x, const double y, const double z, bool make);
        void scale(const double x, const double y, const double z, bool make);
        void rotate_x(const double degree, bool make);
        void rotate_y(const double degree, bool make);
        void rotate_z(const double degree, bool make);

        const matrix44 get_rotate() const;

    private:
        std::tuple<double, double> get_sin_cos(const double degree) const;

        matrix_array row_column;
    };

}
