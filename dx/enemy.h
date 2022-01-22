#pragma once
#include <memory>
#include "polygon_dx.h"

namespace math {
    class matrix44;
}

class enemy final : public polygon_dx {
public:
    // コンストラクタ
    enemy();
    enemy(const enemy&) = default; // コピー
    enemy(enemy&&) = default; // ムーブ

    // デストラクタ
    virtual ~enemy() = default;

    enemy& operator =(const enemy&) = default; // コピー
    enemy& operator =(enemy&&) = default; // ムーブ

    void update() override;
    void render() override;

    void set_hit(const bool hit);
    bool get_hit() const { return hit; }

    void set_billboard(const bool billboard) { this->billboard = billboard; }
    void set_billboard_matrix(const math::matrix44& matrix) { billboard_matrix = matrix; }

private:
    double angle_y;
    bool hit;
    bool end;
    bool billboard;
    math::matrix44 billboard_matrix;
};
