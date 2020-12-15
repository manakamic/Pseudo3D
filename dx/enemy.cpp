#include "vector3.h"
#include "matrix44.h"
#include "enemy.h"
#if _DEBUG_3D
#include "vertex.h"
#include "DxLib.h"
#endif

namespace {
    constexpr auto END_MILLI_SEC = 3000.0;
}

enemy::enemy() : polygon_dx(polygon_dx::type_kind::enemy) {
    angle_y = 0.0;
    hit = false;
    end = false;
}

void enemy::set_hit(const bool hit) {
    this->hit = hit;
    start_time = std::chrono::system_clock::now();
}

void enemy::update() {
    if (end) {
        return;
    }

    if (update_function != nullptr) {
        update_function(this);
    }

    if (world_position != nullptr && world_matrix != nullptr) {
        if (hit) {
            auto delta_time = std::chrono::system_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(delta_time - start_time).count();
            auto rate = static_cast<double>(elapsed) / END_MILLI_SEC;

            if (rate > 1.0) {
                rate = 1.0;
                end = true;
            }

            auto scale = 1.0 - rate;

            world_matrix->rotate_y(angle_y, true);
            world_matrix->scale(scale, scale, scale, false);
            world_matrix->transfer(world_position->get_x(),
                                   world_position->get_y() + (half_size * scale),
                                   world_position->get_z(), false);

            angle_y += 10.0 * scale;

            if (angle_y > 360.0) {
                angle_y -= 360.0;
            }
        }
        else {
            world_matrix->transfer(world_position->get_x(),
                                   world_position->get_y() + half_size,
                                   world_position->get_z(), true);
        }
    }
}

void enemy::render() {
    if (-1 == handle) {
        return;
    }

    if (end) {
        return;
    }

#ifndef _DEBUG_3D
    polygon_dx::render();
#else
    std::array<std::tuple<int, int>, r3d::polygon_vertices_num> xyList;

    auto getXY = [this, &xyList](int index) -> bool {
        auto pos = transform_vertices[index]->get_position();

        if (pos == nullptr) {
            return false;
        }

        std::get<0>(xyList[index]) = static_cast<int>(std::round(pos->get_x()));
        std::get<1>(xyList[index]) = static_cast<int>(std::round(pos->get_y()));

        return true;
    };

    for (auto i = 0; i < r3d::polygon_vertices_num; ++i) {
        getXY(i);
    }

    DrawModiGraph(std::get<0>(xyList[0]), std::get<1>(xyList[0]),
                  std::get<0>(xyList[2]), std::get<1>(xyList[2]),
                  std::get<0>(xyList[3]), std::get<1>(xyList[3]),
                  std::get<0>(xyList[1]), std::get<1>(xyList[1]),
                  handle, TRUE);

    auto color = GetColor(255, 255, 255);
    std::shared_ptr<math::vector3> pos = world_vertices[0];

    DrawFormatString(std::get<0>(xyList[0]), std::get<1>(xyList[0]), color, "%.1lf\n%.1lf\n%.1lf", pos->get_x(), pos->get_y(), pos->get_z());

    auto red = 0;
    auto blue = 255;

    if (hit) {
        red = 255;
        blue = 0;
    }

    color = GetColor(red, 0, blue);

    DrawLine(std::get<0>(xyList[0]), std::get<1>(xyList[0]), std::get<0>(xyList[2]), std::get<1>(xyList[2]), color);
    DrawLine(std::get<0>(xyList[2]), std::get<1>(xyList[2]), std::get<0>(xyList[3]), std::get<1>(xyList[3]), color);
    DrawLine(std::get<0>(xyList[3]), std::get<1>(xyList[3]), std::get<0>(xyList[1]), std::get<1>(xyList[1]), color);
    DrawLine(std::get<0>(xyList[1]), std::get<1>(xyList[1]), std::get<0>(xyList[0]), std::get<1>(xyList[0]), color);
#endif
}
