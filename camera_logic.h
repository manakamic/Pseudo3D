#pragma once
#include <memory>

namespace r3d {
    class camera;
}

namespace math {
    class vector3;
    class matrix44;
}

void process_camera(const std::shared_ptr<r3d::camera>& camera, math::vector3& camera_pos, math::matrix44& camera_mat);
