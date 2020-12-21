#include "constants.h"
#include "utility.h"
#include "vector3.h"
#include "matrix44.h"
#include "shot.h"
#include "enemy.h"
#include "pseudo3d.h"
#include "pseudo3d_logic.h"

namespace {
    constexpr auto ENEMY_RANDOM_X = 25;
    constexpr auto ENEMY_RANDOM_Z = 40;

    constexpr auto ENEMY_MIN = 4;
    constexpr auto ENEMY_MAX = 8;

    constexpr std::array<const TCHAR*, 3> PNG_LIST = { _T("enemy0.png"), _T("enemy1.png"), _T("enemy2.png") };
}

bool initialize_pseudo3d(const std::shared_ptr<pseudo3d>& sp_pseudo3d) {
    // �p�x���烉�W�A���ւ̕ϊ�
    auto fov = math::utility::degree_to_radian(45.0);
    auto near_z = 1.0;
    auto far_z = 500.0;

    if (!sp_pseudo3d->initialize(fov, near_z, far_z, static_cast<double>(SCREEN_WIDTH), static_cast<double>(SCREEN_HEIGHT), false)) {
        return false;
    }

    auto late_update_func = [](const pseudo3d* instance) {
        const std::vector<std::shared_ptr<polygon_dx>>& polygon_list = instance->get_polygon_list();

        // shot ������������
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


            // ���Ƀ��[���h�ړ��ςȂ̂ŏI�_�ƂȂ�
            auto end = shot_type->get_world_position();
            auto move = p_shot->get_move();
            // �I�_����ړ��ʂ�߂��Ύn�_�ƂȂ�
            auto start = (*end) + math::vector3(-move->get_x(), -move->get_y(), -move->get_z());

            // enemy ������������
            for (auto&& enemy_type : polygon_list) {
                if (enemy_type->get_type_kind() == polygon_dx::type_kind::shot) {
                    continue;
                }

                auto p_enemy = dynamic_cast<enemy*>(enemy_type.get());

                if (p_enemy == nullptr) {
                    continue;
                }

                if (p_enemy->get_hit()) {
                    continue;
                }

                auto p0 = enemy_type->get_world_position(0);
                auto p1 = enemy_type->get_world_position(1);
                auto p2 = enemy_type->get_world_position(2);
                auto p3 = enemy_type->get_world_position(3);
                auto result = math::utility::collision_polygon_line(*p0, *p1, *p2, *p3, start, *end);
                bool hit = std::get<0>(result);

                if (hit) {
                    p_enemy->set_hit(true);
                    p_shot->set_end(true);
                }
            }
        }
    };

    sp_pseudo3d->set_late_function(late_update_func);

    return true;
}

void initialize_enemy(const std::shared_ptr<pseudo3d>& pseudo3d) {
    auto enemy_num = math::utility::get_random(ENEMY_MIN, ENEMY_MAX);
    auto png_max = static_cast<int>(PNG_LIST.size()) - 1;

    for (auto i = 0; i < enemy_num; ++i) {
        std::shared_ptr<enemy> enemy(new enemy);
        auto png_index = math::utility::get_random(0, png_max);
        auto offset_x = math::utility::get_random(-ENEMY_RANDOM_X, ENEMY_RANDOM_X);
        auto offset_z = math::utility::get_random(0, ENEMY_RANDOM_Z);
        auto offset = math::vector3(static_cast<double>(offset_x), 0.0, static_cast<double>(offset_z));

        enemy->initialize(PNG_LIST[png_index], ENEMY_SIZE, offset);
        pseudo3d->add_polygon(enemy);
    }
}