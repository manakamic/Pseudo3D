#include "vector4.h"
#include "matrix44.h"
#include "vertex.h"
#include "camera.h"

namespace r3d {

    camera::camera() {
        position = nullptr;
        target = nullptr;
        up = nullptr;

        matrix = nullptr;
    }

    bool camera::initialize() {

        // デフォルトでカメラ値を設定する
        position.reset(new math::vector4(0.0, 0.0, -10.0));
        target.reset(new math::vector4(0.0, 0.0, 0.0));
        up.reset(new math::vector4(0.0, 1.0, 0.0));

        matrix.reset(new math::matrix44);

        look_at();

        return true;
    }

    void camera::look_at() {
        auto pos = *position;
        auto axis_z = (*target) - pos;

        axis_z.normalized();

        auto axis_x = up->cross(axis_z);

        axis_x.normalized();

        auto axis_y = axis_z.cross(axis_x);

        math::matrix_array row_column{ {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}} };

        row_column[0][0] = axis_x.get_x();
        row_column[0][1] = axis_y.get_x();
        row_column[0][2] = axis_z.get_x();
        row_column[0][3] = 0.0;

        row_column[1][0] = axis_x.get_y();
        row_column[1][1] = axis_y.get_y();
        row_column[1][2] = axis_z.get_y();
        row_column[1][3] = 0.0;

        row_column[2][0] = axis_x.get_z();
        row_column[2][1] = axis_y.get_z();
        row_column[2][2] = axis_z.get_z();
        row_column[2][3] = 0.0;

        row_column[3][0] = -axis_x.dot(pos);
        row_column[3][1] = -axis_y.dot(pos);
        row_column[3][2] = -axis_z.dot(pos);
        row_column[3][3] = 1.0;

        matrix.reset(new math::matrix44(row_column));
    }

    bool camera::culling(const math::vector4& position0, const math::vector4& position1, const math::vector4& position2) const {
        // 視線の逆ベクトル
        auto inv_eye = (*position) - (*target);

        inv_eye.normalized();

        // ポリゴン面の法線を求める
        auto v01 = position1 - position0;
        auto v02 = position2 - position0;

        v01.normalized();
        v02.normalized();

        auto poly_normal = v01.cross(v02);

        // 内積でカメラかリング
        if (inv_eye.dot(poly_normal) < 0.0) {
            return false; // 裏面を向いている
        }

        return true;
    }

    bool camera::set_position(const math::vector4& position) {
        if (this->position == nullptr) {
            return false;
        }

        this->position->set(position);

        return true;
    }

    bool camera::set_target(const math::vector4& target) {
        if (this->target == nullptr) {
            return false;
        }

        this->target->set(target);

        return true;
    }

    bool camera::set_position(const double x, const double y, const double z) {
        if (position == nullptr) {
            return false;
        }

        position->set(x, y, z);

        return true;
    }

    bool camera::set_target(const double x, const double y, const double z) {
        if (target == nullptr) {
            return false;
        }

        target->set(x, y, z);

        return true;
    }

}
