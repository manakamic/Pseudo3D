#pragma once
#include <memory>

class pseudo3d;

#if defined(_USE_LIGHTING)
namespace r3d {
    class camera;
}
#endif

bool initialize_pseudo3d(const std::shared_ptr<pseudo3d>& sp_pseudo3d);
#if defined(_USE_LIGHTING)
void initialize_enemy(const std::shared_ptr<pseudo3d>& pseudo3d, std::shared_ptr<r3d::camera>& camera);
#else
void initialize_enemy(const std::shared_ptr<pseudo3d>& pseudo3d);
#endif
