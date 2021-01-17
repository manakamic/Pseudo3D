#include <windows.h>
#include <algorithm>
#include <memory>
#include <cmath>
#include <array>
#include <tchar.h>
#include "DxLib.h"
#include "constants.h"
#include "utility.h"
#include "vector3.h"
#include "matrix44.h"
#include "camera.h"
#include "enemy.h"
#include "pseudo3d.h"
#include "camera_logic.h"
#include "shot_logic.h"
#include "pseudo3d_logic.h"
#if defined(_USE_RASTERIZE)
#include "rasterize.h"
#endif

namespace {
    constexpr auto WINDOW_TITLE = "Pseudo 3D Sample";

    constexpr auto SCREEN_DEPTH = 32;
}


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

#if defined(_USE_RASTERIZE)
    std::shared_ptr<rasterize> rasterize(new rasterize);

    if (!rasterize->initialize(SCREEN_WIDTH, SCREEN_HEIGHT)) {
        DxLib_End();
        return -1;
    }
#endif

    SetDrawScreen(DX_SCREEN_BACK);

    std::shared_ptr<pseudo3d> pseudo3d(new pseudo3d);

    initialize_pseudo3d(pseudo3d);
    initialize_enemy(pseudo3d);

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
#if defined(_USE_RASTERIZE)
        rasterize->clear();
#endif

        pseudo3d->render();

#if defined(_USE_RASTERIZE)
        rasterize->render();
#endif

        ScreenFlip();
    }

    DxLib_End();

    return 0;
}
