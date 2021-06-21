#pragma once
#include "polygon_dx.h"

namespace math {
    class vector4;
}

class shot final : public polygon_dx {
public:
    // コンストラクタ
    shot();
    shot(const shot&) = default; // コピー
    shot(shot&&) = default; // ムーブ

    // デストラクタ
    virtual ~shot() = default;

    shot& operator =(const shot&) = default; // コピー
    shot& operator =(shot&&) = default; // ムーブ

    bool initialize(const TCHAR* file_name, double size, math::vector4& offset) override;
    void update() override;
    void render() override;

    void set_move(const math::vector4& move);

    std::shared_ptr<math::vector4> get_move() const { return move; }

    void set_end(const bool end) { this->end = end; }
    bool get_end() const { return end; }

private:
    std::shared_ptr<math::vector4> move;
    bool end;
};
