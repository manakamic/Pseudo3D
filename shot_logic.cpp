#include "constants.h"
#include "vector3.h"
#include "matrix44.h"
#include "pseudo3d.h"
#include "shot.h"
#include "shot_logic.h"

namespace {
    constexpr auto SHOT_Y = ENEMY_SIZE / 2.0;

    constexpr auto SHOT_SIZE = 2.0;
    constexpr auto SHOT_OFFSET_SIZE = 12.0;
    constexpr auto SHOT_SPEED = 2.5;
    constexpr auto SHOT_MAX = 5;

    constexpr auto SHOT_PNG = _T("shot.png");

    auto shot_num = 0;
}

bool on_shot(const std::shared_ptr<pseudo3d>& pseudo3d, const math::vector3& camera_pos) {
    std::shared_ptr<shot> shot_instance = nullptr;
    shot* p_shot = nullptr;
    auto instance = pseudo3d->get_shot();

    // 弾は最大数以上は作らない
    if (instance == nullptr) {
        if (shot_num >= SHOT_MAX) {
            return false;
        }

        shot_instance = std::make_shared<shot>();
        p_shot = shot_instance.get();
        shot_num++;
    } else {
        p_shot = dynamic_cast<shot*>(instance.get());

        if (p_shot == nullptr) {
            return false;
        }
    }

    // カメラの方向ベクトル
    auto dir = math::vector3(-camera_pos.get_x(), 0.0, -camera_pos.get_z());

    dir.normalized();

    auto offset = math::vector3(camera_pos.get_x(), SHOT_Y, camera_pos.get_z()) + (dir * SHOT_OFFSET_SIZE);

    p_shot->initialize(SHOT_PNG, SHOT_SIZE, offset);

    auto move = dir * SHOT_SPEED;

    p_shot->set_move(move);

    auto func = [move](const polygon_dx* instance) {
        auto world_position = instance->get_world_position();

        world_position->add(move.get_x(), move.get_y(), move.get_z());
    };

    p_shot->set_function(func);

    if (shot_instance != nullptr) {
        pseudo3d->add_polygon(shot_instance);
    }

    return true;
}
