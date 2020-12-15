#pragma once
#include "polygon_dx.h"

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

private:
    double angle_y;
    bool hit;
    bool end;
};
