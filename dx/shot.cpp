#include "vector3.h"
#include "matrix44.h"
#include "shot.h"

namespace {
    constexpr long long LIFE_MILLI_SEC = 5000;
}

shot::shot() : polygon_dx(polygon_dx::type_kind::shot) {
    move = nullptr;
    end = false;
}

bool shot::initialize(const TCHAR* file_name, double size, math::vector3& offset) {
    end = false;
    start_time = std::chrono::system_clock::now();

    return polygon_dx::initialize(file_name, size, offset);
}

void shot::update() {
    if (end) {
        return;
    }

    if (update_function != nullptr) {
        update_function(this);
    }

    if (world_position != nullptr && world_matrix != nullptr) {
        world_matrix->rotate_x(85.0, true); // �����炢�̂ŏ����X����(90�x�ɂ͂��Ȃ�)
        world_matrix->transfer(world_position->get_x(),
                               world_position->get_y(),
                               world_position->get_z(), false);
    }

    // �e�͓G�ɓ����邩���Ԍo�߂Ŗ����ɂ���
    auto delta_time = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(delta_time - start_time).count();

    if (elapsed > LIFE_MILLI_SEC) {
        end = true;
    }
}

void shot::render() {
    if (end) {
        return;
    }

    polygon_dx::render();
}

void shot::set_move(const math::vector3& move) {
    this->move.reset(new math::vector3(move));
}
