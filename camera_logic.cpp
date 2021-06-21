#include "constants.h"
#include "vector4.h"
#include "matrix44.h"
#include "camera.h"
#include "camera_logic.h"
#include "DxLib.h"

namespace {
    constexpr auto CAMERA_MOVO_SIZE = 0.5;
}

void process_camera(const std::shared_ptr<r3d::camera>& camera, math::vector4& camera_pos, math::matrix44& camera_mat) {
    auto set_pos = false;
    auto move = [&camera_pos](bool up) {
        // カメラの方向ベクトル
        auto dir = math::vector4(-camera_pos.get_x(), 0.0, -camera_pos.get_z());
        auto size = up ? CAMERA_MOVO_SIZE : -CAMERA_MOVO_SIZE;

        dir.normalized();

        camera_pos = camera_pos + (dir * size);
    };
    auto rotate = [&camera_pos, &camera_mat](bool left) {
        auto rot = left ? 1.0 : -1.0;

        camera_mat.rotate_y(rot, true); // 1 or -1 度の Y 軸回転行列
        camera_pos = camera_pos * camera_mat;
    };

    if (1 == CheckHitKey(KEY_INPUT_UP)) {
        move(true);
        set_pos = true;
    } else if (1 == CheckHitKey(KEY_INPUT_DOWN)) {
        move(false);
        set_pos = true;
    } else if (1 == CheckHitKey(KEY_INPUT_LEFT)) {
        rotate(true);
        set_pos = true;
    } else if (1 == CheckHitKey(KEY_INPUT_RIGHT)) {
        rotate(false);
        set_pos = true;
    } else if (1 == CheckHitKey(KEY_INPUT_SPACE)) {
        camera_pos.set(CAMERA_START_X, CAMERA_START_Y, CAMERA_START_Z); // リセット
        set_pos = true;
    }

    if (set_pos) {
        camera->set_position(camera_pos);
    }
}
