#pragma once
#include <memory>

namespace math {
    class vector4;
}

class pseudo3d;

bool on_shot(const std::shared_ptr<pseudo3d>& pseudo3d, const math::vector4& camera_pos);
