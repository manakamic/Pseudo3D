#pragma once
#include <memory>

class pseudo3d;

bool initialize_pseudo3d(const std::shared_ptr<pseudo3d>& sp_pseudo3d);
void initialize_enemy(const std::shared_ptr<pseudo3d>& pseudo3d);
