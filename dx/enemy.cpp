#include "vector4.h"
#include "matrix44.h"
#include "enemy.h"

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

    polygon_dx::render();
}
