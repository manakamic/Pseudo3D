#include <cmath>
#include <tuple>
#include "vector3.h"
#include "matrix44.h"
#include "vertex.h"
#include "polygon_dx.h"
#include "DxLib.h"

namespace {
    std::unordered_map<const TCHAR*, int> handle_list;
}

polygon_dx::polygon_dx(type_kind type) {
    this->type = type;
    handle = -1;
    half_size = 0.0;
    culling = false;
    world_position = nullptr;
    world_matrix = nullptr;
    update_function = nullptr;
    world_vertices.fill(nullptr);
}

bool polygon_dx::initialize(const TCHAR* file_name, double size, math::vector3& offset) {
    auto init = r3d::polygon::initialize();
    auto image = load_image(file_name);

    if (!init || !image) {
        return false;
    }

    world_position.reset(new math::vector3(offset));
    world_matrix.reset(new math::matrix44);

    half_size = size / 2.0;
    
    std::array<std::shared_ptr<math::vector3>, r3d::polygon_vertices_num> position_list = {
         std::make_shared<math::vector3>(-half_size,  half_size, 0.0),
         std::make_shared<math::vector3>(-half_size, -half_size, 0.0),
         std::make_shared<math::vector3>( half_size,  half_size, 0.0),
         std::make_shared<math::vector3>( half_size, -half_size, 0.0)
    };
    std::array<std::shared_ptr<r3d::vertex>, r3d::polygon_vertices_num> vertices_list = {
        nullptr,
        nullptr,
        nullptr,
        nullptr
    };

    for (int i = 0; i < r3d::polygon_vertices_num; ++i) {
        std::shared_ptr<r3d::vertex> vertex(new r3d::vertex);

        vertex->initialize();
        vertex->set_position(*position_list[i]);
        vertices_list[i] = vertex;
    }

    set_vertices(vertices_list[0], vertices_list[1], vertices_list[2], vertices_list[3]);

    return true;
}

bool polygon_dx::load_image(const TCHAR* file_name) {
    auto find = handle_list.find(file_name);

    if (find == handle_list.end()) {
        handle = LoadGraph(file_name);

        if (-1 != handle) {
            handle_list.emplace(file_name, handle);
            //handle_list[file_name] = handle;

            return true;
        }

        return false;
    }

    handle = handle_list[file_name];

    return true;
}

void polygon_dx::update() {
    if (update_function != nullptr) {
        update_function(this);
    }
}

void polygon_dx::render() {
    if (-1 == handle) {
        return;
    }

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
}

bool polygon_dx::transform(const math::matrix44& matrix, const bool transform) {
    for (auto i = 0; i < r3d::polygon_vertices_num; ++i) {
        std::shared_ptr<r3d::vertex> src = transform ? transform_vertices[i] : vertices[i];
        std::shared_ptr<r3d::vertex> dst = transform_vertices[i];

        if (src == nullptr || dst == nullptr || world_matrix == nullptr) {
            return false;
        }

        auto src_pos = src->get_position();

        if (src_pos == nullptr) {
            return false;
        }

        auto world_pos = (*src_pos) * (*world_matrix);
        auto trans_pos = world_pos.mult_with_w(matrix);

        world_vertices[i].reset(new math::vector3(world_pos));

        dst->set_position(trans_pos);
    }

    return true;
}
