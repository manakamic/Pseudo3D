#pragma once

#include <memory>
#include <vector>
#include <functional>

namespace r3d {
    class camera;
    class perspective;
    class viewport;
}

namespace math {
    class matrix44;
}

class polygon_dx;

class pseudo3d {
public:
    // コンストラクタ
    pseudo3d();
    pseudo3d(const pseudo3d&) = default; // コピー
    pseudo3d(pseudo3d&&) = default; // ムーブ

    // デストラクタ
    virtual ~pseudo3d() = default;

    pseudo3d& operator =(const pseudo3d&) = default; // コピー
    pseudo3d& operator =(pseudo3d&&) = default; // ムーブ

    bool initialize(const double fov_y, const double near_z, const double far_z, const double width, const double height, const bool culling);
    void update();
    void render();

    void add_polygon(const std::shared_ptr<polygon_dx>& polygon);

    std::shared_ptr<polygon_dx> get_shot() const;

    const std::shared_ptr<r3d::camera> get_camera() const { return camera; }
    const std::vector<std::shared_ptr<polygon_dx>>& get_polygon_list() const { return polygon_list; }

    void set_function(const std::function<void(const pseudo3d* instance)> function) { update_function = function; }
    void set_late_function(const std::function<void(const pseudo3d* instance)> function) { late_update_function = function; }

protected:
    void transform(const std::shared_ptr<math::matrix44>& camera_matrix);

    std::function<void(const pseudo3d* instance)> update_function;
    std::function<void(const pseudo3d* instance)> late_update_function;

    std::shared_ptr<r3d::camera> camera;
    std::shared_ptr<r3d::perspective> perspective;
    std::shared_ptr<r3d::viewport> viewport;

    std::vector<std::shared_ptr<polygon_dx>> polygon_list;

    math::matrix44 perspective_viewport;

    bool culling;
};
