#pragma once

#include <tchar.h>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include "polygon.h"

namespace math {
    class vector3;
    class matrix44;
}

class polygon_dx : public r3d::polygon {
public:
    enum class type_kind : int {
        none = 0,
        enemy,
        shot,
        max
    };

    // コンストラクタ
    polygon_dx(type_kind type);
    polygon_dx(const polygon_dx&) = default; // コピー
    polygon_dx(polygon_dx&&) = default; // ムーブ

    // デストラクタ
    virtual ~polygon_dx();

    polygon_dx& operator =(const polygon_dx&) = default; // コピー
    polygon_dx& operator =(polygon_dx&&) = default; // ムーブ

    type_kind get_type_kind() const { return type; }

    virtual bool initialize(const TCHAR* file_name, double size, math::vector3& offset);
    virtual void update();
    virtual void render();

    bool transform(const math::matrix44& matrix, const bool transform) override;

    bool is_culling() const { return culling; }
    void set_culling(const bool culling) { this->culling = culling; }

    std::shared_ptr<math::vector3> get_world_position() const { return world_position; }
    std::shared_ptr<math::vector3> get_world_position(const int index) const { return world_vertices[index]; }

    void set_function(const std::function<void(const polygon_dx* instance)> function) { update_function = function; }

protected:
    bool load_image(const TCHAR* file_name);

    type_kind type;

    int handle;

    double half_size;

    bool culling;

    std::shared_ptr<math::vector3> world_position;
    std::unique_ptr<math::matrix44> world_matrix;

    std::function<void(const polygon_dx* instance)> update_function;

    std::array<std::shared_ptr<math::vector3>, r3d::polygon_vertices_num> world_vertices;

    std::chrono::system_clock::time_point start_time;
};
