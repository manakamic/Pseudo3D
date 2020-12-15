#include <windows.h>
#include <algorithm>
#include <memory>
#include <cmath>
#include <array>
#include <tchar.h>
#include "DxLib.h"
#include "utility.h"
#include "vector3.h"
#include "matrix44.h"
#include "camera.h"
#include "vertex.h"
#include "enemy.h"
#include "shot.h"
#include "pseudo3d.h"

namespace {
    constexpr auto WINDOW_TITLE = "Pseudo 3D Sample";
    constexpr auto SCREEN_WIDTH = 1280;
    constexpr auto SCREEN_HEIGHT = 720;
    constexpr auto SCREEN_DEPTH = 32;

    constexpr auto CAMERA_MOVO_SIZE = 0.5;

    constexpr auto CAMERA_START_X = 0.0;
    constexpr auto CAMERA_START_Y = 10.0;
    constexpr auto CAMERA_START_Z = -100.0;

    constexpr auto ENEMY_SIZE = 10.0;

    constexpr auto ENEMY_RANDOM_X = 25;
    constexpr auto ENEMY_RANDOM_Z = 40;

    constexpr auto ENEMY_MIN = 4;
    constexpr auto ENEMY_MAX = 8;

    constexpr auto SHOT_Y = ENEMY_SIZE / 2.0;

    constexpr auto SHOT_SIZE = 2.0;
    constexpr auto SHOT_OFFSET_SIZE = 12.0;
    constexpr auto SHOT_SPEED = 2.5;
    constexpr auto SHOT_MAX = 5;

    auto shot_num = 0;

    constexpr std::array<const TCHAR*, 3> PNG_LIST = { _T("enemy0.png"), _T("enemy1.png"), _T("enemy2.png") };
    constexpr auto SHOT_PNG = _T("shot.png");
}

bool initialize_pseudo3d(const std::shared_ptr<pseudo3d>& sp_pseudo3d);
void process_camera(const std::shared_ptr<r3d::camera>& camera, math::vector3& camera_pos, math::matrix44& camera_mat);
bool on_shot(const std::shared_ptr<pseudo3d>& pseudo3d, const math::vector3& camera_pos);

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    auto window_mode = FALSE;

#ifdef _DEBUG
    window_mode = TRUE;
#endif

    SetMainWindowText(WINDOW_TITLE);

    ChangeWindowMode(window_mode);

    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH);

    if (DxLib_Init() == -1) {
        return -1;
    }

    SetDrawScreen(DX_SCREEN_BACK);

    std::shared_ptr<pseudo3d> pseudo3d(new pseudo3d);

    initialize_pseudo3d(pseudo3d);

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

    auto camera = pseudo3d->get_camera();
    auto camera_pos = new math::vector3(CAMERA_START_X, CAMERA_START_Y, CAMERA_START_Z);
    auto camera_mat = new math::matrix44();

    camera->set_position(*camera_pos);
    camera->set_target(0.0, CAMERA_START_Y, 0.0);

    auto last_input_s = false;

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        if (1 == CheckHitKey(KEY_INPUT_S)) {
            if (!last_input_s) {
                on_shot(pseudo3d, *camera_pos);
            }

            last_input_s = true;
        }
        else {
            last_input_s = false;
        }

        process_camera(camera, *camera_pos, *camera_mat);

        pseudo3d->update();

        ClearDrawScreen();

        pseudo3d->render();

        ScreenFlip();
    }

    DxLib_End();

    return 0;
}

bool initialize_pseudo3d(const std::shared_ptr<pseudo3d>& sp_pseudo3d) {
    // 角度からラジアンへの変換
    auto fov = math::utility::degree_to_radian(45.0);
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
            auto start = (*end) + math::vector3(-move->get_x(), -move->get_y(), -move->get_z());

            // enemy だけ処理する
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

void process_camera(const std::shared_ptr<r3d::camera>& camera, math::vector3& camera_pos, math::matrix44& camera_mat) {
    auto set_pos = false;
    auto move = [&camera_pos](bool up) {
        // カメラの方向ベクトル
        auto dir = math::vector3(-camera_pos.get_x(), 0.0, -camera_pos.get_z());
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
    }
    else {
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
