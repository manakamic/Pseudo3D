#include <algorithm>
#include "constants.h"
#include "utility.h"
#include "vector4.h"
#include "matrix44.h"
#include "shot.h"
#include "enemy.h"
#include "pseudo3d.h"
#include "pseudo3d_logic.h"
#if defined(_USE_LIGHTING)
#include "camera.h"
#endif

namespace {
    constexpr auto ENEMY_RANDOM_X = 25;
    constexpr auto ENEMY_RANDOM_Z = 40;

    constexpr auto ENEMY_MIN = 4;
    constexpr auto ENEMY_MAX = 8;

    constexpr std::array<const TCHAR*, 3> PNG_LIST = { _T("enemy0.png"), _T("enemy1.png"), _T("enemy2.png") };
}

bool initialize_pseudo3d(const std::shared_ptr<pseudo3d>& sp_pseudo3d) {
    // 角度からラジアンへの変換
    auto fov = math::utility::degree_to_radian(60.0);
    auto near_z = 1.0;
    auto far_z = 500.0;

    if (!sp_pseudo3d->initialize(fov, near_z, far_z, static_cast<double>(SCREEN_WIDTH), static_cast<double>(SCREEN_HEIGHT), false)) {
        return false;
    }

    auto late_update_func = [](const pseudo3d* instance) {
        const std::vector<std::shared_ptr<polygon_dx>>& polygon_list = instance->get_polygon_list();

        // shot だけ処理する
        for (auto&& shot_type : polygon_list) {
            if (shot_type->get_type_kind() == polygon_dx::type_kind::enemy) {
                continue;
            }

            auto p_shot = dynamic_cast<shot*>(shot_type.get());

            if (p_shot == nullptr) {
                continue;
            }

            if (p_shot->get_end()) {
                continue;
            }


            // 既にワールド移動済なので終点となる
            auto end = shot_type->get_world_position();
            auto move = p_shot->get_move();
            // 終点から移動量を戻せば始点となる
            auto start = (*end) - (*move);

            // enemy だけ処理する + polygon_list を逆から処理する
            std::for_each(polygon_list.rbegin(), polygon_list.rend(), [&p_shot, &end, &start](auto&& enemy_type) {
                if (!p_shot->get_end() && enemy_type->get_type_kind() == polygon_dx::type_kind::enemy) {
                    auto p_enemy = dynamic_cast<enemy*>(enemy_type.get());

                    if (p_enemy != nullptr && !p_enemy->get_hit()) {
                        auto p0 = enemy_type->get_world_position(0);
                        auto p1 = enemy_type->get_world_position(1);
                        auto p2 = enemy_type->get_world_position(2);
                        auto p3 = enemy_type->get_world_position(3);
                        auto result = math::utility::collision_polygon_line(*p0, *p1, *p2, *p3, start, *end);
                        auto hit = std::get<0>(result);

                        if (hit) {
                            p_enemy->set_hit(true);
                            p_shot->set_end(true);
                        }
                    }
                }
            });
        }
    };

    sp_pseudo3d->set_late_function(late_update_func);

    return true;
}

#if defined(_USE_LIGHTING)
void initialize_enemy(const std::shared_ptr<pseudo3d>& pseudo3d, std::shared_ptr<r3d::camera>& camera) {
#else
void initialize_enemy(const std::shared_ptr<pseudo3d>&pseudo3d) {
#endif
    auto enemy_num = math::utility::get_random(ENEMY_MIN, ENEMY_MAX);
    auto png_max = static_cast<int>(PNG_LIST.size()) - 1;
    static auto is_left = false;
    auto update_enemy = [](const polygon_dx* instance) {
        auto dx_instance = const_cast<polygon_dx*>(instance);
        auto enemy_instance = dynamic_cast<const enemy*>(dx_instance);

        if (enemy_instance->get_hit()) {
            return;
        }

        auto world_position = instance->get_world_position();
    };

    for (auto i = 0; i < enemy_num; ++i) {
        std::shared_ptr<enemy> enemy_ptr(new enemy);
        auto png_index = math::utility::get_random(0, png_max);
        auto offset_x = math::utility::get_random(-ENEMY_RANDOM_X, ENEMY_RANDOM_X);
        auto offset_z = math::utility::get_random(0, ENEMY_RANDOM_Z);
        auto offset = math::vector4(static_cast<double>(offset_x), 0.0, static_cast<double>(offset_z));

        enemy_ptr->initialize(PNG_LIST[png_index], ENEMY_SIZE, offset);
        enemy_ptr->set_function(update_enemy);
        pseudo3d->add_polygon(enemy_ptr);

#if defined(_USE_LIGHTING)
        enemy_ptr->set_camera(camera);
#endif
    }
}
