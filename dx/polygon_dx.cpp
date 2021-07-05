#include <cmath>
#include <tuple>
#include "vector4.h"
#include "matrix44.h"
#include "vertex.h"
#include "polygon_dx.h"
#include "DxLib.h"
#if defined(_USE_RASTERIZE)
#include "png.hpp"
#include "rasterize.h"
#endif
#if defined(_USE_LIGHTING)
#include "color.h"
#endif

namespace {
    std::unordered_map<const TCHAR*, int> handle_list;
#if defined(_USE_RASTERIZE)
    std::unordered_map<int, png::image <png::rgba_pixel>> image_list;
    auto name_counter = 0;
#endif
#if defined(_USE_NORMAL_MAP)
    constexpr const TCHAR* normal_map_suffix = _T("_normal");
#endif
}

polygon_dx::polygon_dx(type_kind type) {
    this->type = type;
    handle = -1;
#if defined(_USE_NORMAL_MAP)
    handle_normal_map = -1;
#endif
    half_size = 0.0;
    culling = false;
    world_position = nullptr;
    world_matrix = nullptr;
    update_function = nullptr;
    world_vertices.fill(nullptr);
}

polygon_dx::~polygon_dx() {
#if !defined(_USE_RASTERIZE)
    if (handle != -1) {
        for (auto&& pair : handle_list) {
            if (pair.second == handle) {
                DeleteGraph(handle);
                break;
            }
        }

        handle = -1;
    }
#endif
}

bool polygon_dx::initialize(const TCHAR* file_name, double size, math::vector4& offset) {
    auto init = r3d::polygon::initialize();
    auto image = load_image(file_name, handle);

    if (!init || !image) {
        return false;
    }

#if defined(_USE_NORMAL_MAP)
    auto file = std::string(file_name);
    auto suffix = std::string(normal_map_suffix);
    auto normal_map_file = file.insert(6, suffix);
    auto image_normal_map = load_image(normal_map_file.c_str(), handle_normal_map);

    if (!image_normal_map) {
        return false;
    }
#endif

    world_position.reset(new math::vector4(offset));
    world_matrix.reset(new math::matrix44);

    half_size = size / 2.0;

    std::array<std::shared_ptr<math::vector4>, r3d::polygon_vertices_num> position_list = {
         std::make_shared<math::vector4>(-half_size,  half_size, 0.0),
         std::make_shared<math::vector4>(-half_size, -half_size, 0.0),
         std::make_shared<math::vector4>( half_size,  half_size, 0.0),
         std::make_shared<math::vector4>( half_size, -half_size, 0.0)
    };
    std::array<std::shared_ptr<r3d::vertex>, r3d::polygon_vertices_num> vertices_list = {
        nullptr,
        nullptr,
        nullptr,
        nullptr
    };
#if defined(_USE_RASTERIZE)
    std::array<double, r3d::polygon_vertices_num> u_list = { 0.0, 0.0, 1.0, 1.0 };
    std::array<double, r3d::polygon_vertices_num> v_list = { 0.0, 1.0, 0.0, 1.0 };
#endif
#if defined(_USE_LIGHTING)
    auto normal = math::vector4(0.0, 0.0, -1.0);
#if defined(_USE_NORMAL_MAP)
    auto tangent = math::vector4(1.0, 0.0, 0.0);
    auto binormal = math::vector4(1.0, 1.0, 0.0);
#endif
    auto diffuse = image::color();
    auto speculer = image::color();
    auto speculer_power = 650.0;
#endif

    for (int i = 0; i < r3d::polygon_vertices_num; ++i) {
        std::shared_ptr<r3d::vertex> vertex(new r3d::vertex);

        vertex->initialize();
        vertex->set_position(*position_list[i]);
#if defined(_USE_RASTERIZE)
        vertex->set_uv(u_list[i], v_list[i]);
#endif
#if defined(_USE_LIGHTING)
        vertex->set_normal(normal);
#if defined(_USE_NORMAL_MAP)
        vertex->set_tangent(tangent);
        vertex->set_binormal(binormal);
#endif
        vertex->set_diffuse(diffuse);
        vertex->set_speculer(speculer, speculer_power);
#endif
        vertices_list[i] = vertex;
    }

    set_vertices(vertices_list[0], vertices_list[1], vertices_list[2], vertices_list[3]);

    return true;
}

bool polygon_dx::load_image(const TCHAR* file_name, int& get_handle) {
    auto find = handle_list.find(file_name);

    if (find == handle_list.end()) {
#if defined(_USE_RASTERIZE)
        try {
            png::image <png::rgba_pixel> image(file_name);

            get_handle = name_counter;
            name_counter++;

            image_list.emplace(get_handle, image);
        } catch (png::error& error) {
            return false;
        }

        handle_list.emplace(file_name, get_handle);

        return true;
#else
        get_handle = LoadGraph(file_name);

        if (-1 != get_handle) {
            handle_list.emplace(file_name, get_handle);
            //handle_list[file_name] = get_handle;

            return true;
        }

        return false;
#endif
    }

    get_handle = handle_list[file_name];

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

#if defined(_USE_RASTERIZE)
#if defined(_USE_LIGHTING)
#if defined(_USE_NORMAL_MAP)
    rasterize::Draw(transform_vertices, image_list[handle], image_list[handle_normal_map], camera_ptr);
#else
    rasterize::Draw(transform_vertices, image_list[handle], camera_ptr);
#endif
#else
    rasterize::Draw(transform_vertices, image_list[handle]);
#endif
#else
    DrawModiGraph(std::get<0>(xyList[0]), std::get<1>(xyList[0]),
                  std::get<0>(xyList[2]), std::get<1>(xyList[2]),
                  std::get<0>(xyList[3]), std::get<1>(xyList[3]),
                  std::get<0>(xyList[1]), std::get<1>(xyList[1]),
                  handle, TRUE);
#endif

#if defined(_DEBUG_3D)
    std::shared_ptr<math::vector4> pos = world_vertices[0];
    auto color = GetColor(255, 255, 255);

    DrawFormatString(std::get<0>(xyList[0]), std::get<1>(xyList[0]), color, "%.1lf\n%.1lf\n%.1lf", pos->get_x(), pos->get_y(), pos->get_z());

    color = GetColor(0, 0, 255);

    DrawLine(std::get<0>(xyList[0]), std::get<1>(xyList[0]), std::get<0>(xyList[2]), std::get<1>(xyList[2]), color);
    DrawLine(std::get<0>(xyList[2]), std::get<1>(xyList[2]), std::get<0>(xyList[3]), std::get<1>(xyList[3]), color);
    DrawLine(std::get<0>(xyList[3]), std::get<1>(xyList[3]), std::get<0>(xyList[1]), std::get<1>(xyList[1]), color);
    DrawLine(std::get<0>(xyList[1]), std::get<1>(xyList[1]), std::get<0>(xyList[0]), std::get<1>(xyList[0]), color);
#endif
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

        world_vertices[i].reset(new math::vector4(world_pos));

        dst->set_position(trans_pos);
#if defined(_USE_RASTERIZE)
        dst->set_uv(src->get_uv());
#endif
#if defined(_USE_LIGHTING)
        auto src_normal = src->get_normal();

        if (src_normal == nullptr) {
            return false;
        }

        // 法線はワールドマトリクスの回転成分のみを適応
        auto world_rotate = world_matrix->get_rotate();
        auto world_normal = (*src_normal) * world_rotate;

        dst->set_world_position(world_pos);
        dst->set_normal(world_normal);

#if defined(_USE_NORMAL_MAP)
        auto src_tangent = src->get_tangent();
        auto src_binormal = src->get_binormal();

        if (src_tangent == nullptr || src_binormal == nullptr) {
            return false;
        }

        auto world_tangent = (*src_tangent) * world_rotate;
        auto world_binormal = (*src_binormal) * world_rotate;

        dst->set_tangent(world_tangent);
        dst->set_binormal(world_binormal);
#endif

        dst->set_diffuse(*src->get_diffuse());
        dst->set_speculer(*src->get_speculer(), src->get_speculer_power());
#endif
    }

    return true;
}
