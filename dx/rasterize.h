#pragma once

#include <array>
#include <memory>
#include <thread>
#include "png.hpp"

namespace r3d {
    class vertex;
}

class rasterize {
public:
    // DrawModiGraph の代わりにコールする
    static void Draw(const std::array<std::shared_ptr<r3d::vertex>, 4>& vertices, const png::image <png::rgba_pixel>& image);

    // コンストラクタ
    rasterize() = default;
    rasterize(const rasterize&) = default; // コピー
    rasterize(rasterize&&) = default; // ムーブ

    // デストラクタ
    virtual ~rasterize();

    bool initialize(int width, int height);
    void clear();
    void render();
};
