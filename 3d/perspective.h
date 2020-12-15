#pragma once

#include <memory>

namespace math {
    class matrix44;
}

namespace r3d {

    class perspective {
    public:
        // コンストラクタ
        perspective();
        perspective(const perspective&) = default; // コピー
        perspective(perspective&&) = default; // ムーブ

        // デストラクタ
        virtual ~perspective() = default;

        perspective& operator =(const perspective&) = default; // コピー
        perspective& operator =(perspective&&) = default; // ムーブ

        bool initialize(const double fov_y, const double aspect, const double near_z, const double far_z);

        std::shared_ptr<math::matrix44> get_matrix() const { return matrix; }

    private:
        double fov_y;
        double aspect;
        double near_z;
        double far_z;

        std::shared_ptr<math::matrix44> matrix;
    };

}
