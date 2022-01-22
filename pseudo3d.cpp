#include <algorithm>
#include "vector4.h"
#include "matrix44.h"
#include "camera.h"
#include "perspective.h"
#include "viewport.h"
#include "vertex.h"
#include "polygon_dx.h"
#include "shot.h"
#include "pseudo3d.h"
#include "enemy.h"
#if defined(_DEBUG_3D)
#include "utility.h"
#include "DxLib.h"

namespace {
    math::matrix44 debug_cam_pers_view_mat;
}
#endif

pseudo3d::pseudo3d() {
    update_function = nullptr;
    late_update_function = nullptr;

    camera = nullptr;
    perspective = nullptr;
    viewport = nullptr;

    polygon_list.clear();

    perspective_viewport.unit();

    culling = false;
}

bool pseudo3d::initialize(const double fov_y, const double near_z, const double far_z, const double width, const double height, const bool culling) {
    camera.reset(new r3d::camera);
    perspective.reset(new r3d::perspective);
    viewport.reset(new r3d::viewport);

    polygon_list.clear();

    auto init_camera = camera->initialize();
    auto aspect = height / width;
    auto init_perspective = perspective->initialize(fov_y, aspect, near_z, far_z);
    auto init_viewport = viewport->initialize(width, height);

    auto pers_mat = perspective->get_matrix();
    auto view_mat = viewport->get_matrix();

    perspective_viewport = (*pers_mat) * (*view_mat);

    this->culling = culling;

    return init_camera && init_perspective && init_viewport;
}

void pseudo3d::update() {
    if (camera == nullptr) {
        return;
    }

    camera->look_at();

    auto view_matrix = camera->get_matrix();

#if defined(_USE_BILLBOARD)
    auto inverse = view_matrix->get_inverse();

    inverse.set_value(3, 0, 0.0);
    inverse.set_value(3, 1, 0.0);
    inverse.set_value(3, 2, 0.0);
#endif

    for (auto&& polygon : polygon_list) {
#if defined(_USE_BILLBOARD)
        auto enemy_pointer = std::dynamic_pointer_cast<enemy>(polygon);

        if (enemy_pointer != nullptr) {
            enemy_pointer->set_billboard(true);
            enemy_pointer->set_billboard_matrix(inverse);
        }
#endif

        polygon->update();
    }

    if (update_function != nullptr) {
        update_function(this);
    }

    transform(view_matrix);

    if (late_update_function != nullptr) {
        late_update_function(this);
    }
}

void pseudo3d::transform(const std::shared_ptr<math::matrix44>& camera_matrix) {
    if (0 == polygon_list.size()) {
        return;
    }

    if (camera_matrix == nullptr ||  perspective == nullptr || viewport == nullptr) {
        return;
    }

    auto cam_pers_view_mat = (*camera_matrix) * perspective_viewport;

#if defined(_DEBUG_3D)
    debug_cam_pers_view_mat = cam_pers_view_mat;
#endif

    for (auto&& polygon : polygon_list) {
        if (culling && (polygon->get_type_kind() == polygon_dx::type_kind::enemy)) {
            // カリング指定がある場合はポリゴンの法線で向きを判定する
            auto v0 = polygon->get_vertex(0)->get_position();
            auto v1 = polygon->get_vertex(1)->get_position();
            auto v2 = polygon->get_vertex(2)->get_position();
            auto is_culling = camera->culling(*v0, *v1, *v2);

            polygon->set_culling(is_culling);

            if (!is_culling) {
                polygon->transform(cam_pers_view_mat, false);
            }
        }
        else {
            polygon->transform(cam_pers_view_mat, false);
        }
    }

#if !defined(_USE_RASTERIZE)
    if (polygon_list.size() > 1) {
        // ポリゴンの重心の Z 値でソート
        auto compare = [](const std::shared_ptr<polygon_dx>& lhs, const std::shared_ptr<polygon_dx>& rhs) -> bool {
            auto lhs_center = math::vector4();
            auto rhs_center = math::vector4();

            lhs->get_center(lhs_center, true);
            rhs->get_center(rhs_center, true);

            return lhs_center.get_z() > rhs_center.get_z();
        };

        std::sort(polygon_list.begin(), polygon_list.end(), compare);
    }
#endif
}

void pseudo3d::render() {
    if (0 == polygon_list.size()) {
        return;
    }

    for (auto&& polygon : polygon_list) {
        if (!polygon->is_culling()) {
            polygon->render();
        }
    }

#if defined(_DEBUG_3D)
    if (math::utility::collision_point != nullptr) {
        auto debug_point = math::utility::collision_point->mult_with_w(debug_cam_pers_view_mat);
        auto debug_x = debug_point.get_x();
        auto debug_y = debug_point.get_y();
        auto debug_size = 10.0;


        DrawBox(static_cast<int>(debug_x - debug_size), static_cast<int>(debug_y - debug_size),
                static_cast<int>(debug_x + debug_size), static_cast<int>(debug_y + debug_size), GetColor(255, 0, 0), TRUE);
    }
#endif
}

void pseudo3d::add_polygon(const std::shared_ptr<polygon_dx>& polygon) {
    polygon_list.emplace_back(polygon);
}

std::shared_ptr<polygon_dx> pseudo3d::get_shot() const {
    for (auto&& polygon : polygon_list) {
        if (polygon->get_type_kind() == polygon_dx::type_kind::enemy) {
            continue;
        }

        auto p_shot = dynamic_cast<shot*>(polygon.get());

        if (p_shot == nullptr) {
            continue;
        }

        const auto end = p_shot->get_end();

        if (end) {
            return polygon;
        }
    }

    return nullptr;
}
