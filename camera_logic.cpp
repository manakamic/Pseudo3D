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
    // カメラの方向ベクトル
    auto target = *camera->get_target();
    auto dir = camera->get_direction();

    auto move = [&camera, &camera_pos, target, dir](bool up) {
        auto size = up ? CAMERA_MOVO_SIZE : -CAMERA_MOVO_SIZE;
        auto moved = dir * size;

        camera_pos = camera_pos + moved;

        camera->set_position(camera_pos);

        auto new_target = target + moved;

        camera->set_target(new_target.get_x(), new_target.get_y(), new_target.get_z());
    };
    auto rotate = [&camera, &camera_pos, &camera_mat, dir](bool left) {
        auto rot = left ? -1.0 : 1.0;

        camera_mat.rotate_y(rot, true); // 1 or -1 度の Y 軸回転行列

        auto new_dir = dir * camera_mat;

        new_dir.normalized();

        auto new_target = camera_pos + new_dir;

        camera->set_target(new_target.get_x(), new_target.get_y(), new_target.get_z());
    };

    if (1 == CheckHitKey(KEY_INPUT_UP)) {
        move(true);
    } else if (1 == CheckHitKey(KEY_INPUT_DOWN)) {
        move(false);
    } else if (1 == CheckHitKey(KEY_INPUT_LEFT)) {
        rotate(true);
    } else if (1 == CheckHitKey(KEY_INPUT_RIGHT)) {
        rotate(false);
    } else if (1 == CheckHitKey(KEY_INPUT_SPACE)) {
        camera_pos.set(CAMERA_START_X, CAMERA_START_Y, CAMERA_START_Z); // リセット
        camera->set_position(camera_pos);
        camera->set_target(0.0, CAMERA_START_Y, 0.0);
    }
}
