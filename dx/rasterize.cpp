#include <memory>
#include <fstream>
#include "vector3.h"
#include "utility.h"
#include "DxLib.h"
#include "rasterize.h"

namespace {
    constexpr auto BUFFER_PNG_NAME = _T("buffer.png");
    constexpr auto TEST_PNG_NAME = _T("test.png");

    png::image<png::rgba_pixel> buffer;

    unsigned char* on_memoey_png = nullptr;
    auto on_memoey_png_size = 0;
}

void rasterize::Draw(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, png::image <png::rgba_pixel>& image) {
    auto width = buffer.get_width();
    auto height = buffer.get_height();

    if (width <= 0 || height <= 0) {
        return;
    }

    // 指定の矩形で 3 角形を 2 つ作る
    auto p1 = math::vector3(static_cast<float>(x1), static_cast<float>(y1), 0.0f);
    auto p2 = math::vector3(static_cast<float>(x2), static_cast<float>(y2), 0.0f);
    auto p3 = math::vector3(static_cast<float>(x3), static_cast<float>(y3), 0.0f);
    auto p4 = math::vector3(static_cast<float>(x4), static_cast<float>(y4), 0.0f);

    // TODO : 4 頂点を含む最小の矩形でループさせる
    for (auto y = 0U; y < height; ++y) {
        for (auto x = 0U; x < width; ++x) {
            auto p = math::vector3(static_cast<float>(x), static_cast<float>(y), 0.0f);

            // 各 3 角形の内側のピクセルなら処理する
            if (math::utility::inside_triangle_point(p1, p4, p2, p)) {
                // TODO : TEST
                buffer[y][x] = png::rgba_pixel(0xff, 0x00, 0x00, 0xff);
            }
            else if (math::utility::inside_triangle_point(p4, p3, p2, p)) {
                // TODO : TEST
                buffer[y][x] = png::rgba_pixel(0x00, 0x00, 0xff, 0xff);
            }
        }
    }
}

rasterize::rasterize() {
    handle = -1;
}

rasterize::~rasterize() {
    if (handle != -1) {
        DeleteGraph(handle);
        handle = -1;
    }

    if (on_memoey_png != nullptr) {
        delete[] on_memoey_png;
        on_memoey_png = nullptr;
    }
}

bool rasterize::initialize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return false;
    }

    /*
    if (!load_buffer_png()) {
        return false;
    }

    if (!create_dx_handle()) {
        return false;
    }
    */

    png::image<png::rgba_pixel> image(width, height);

    buffer = std::move(image);

    return true;
}

void rasterize::clear() {
    for (auto y = 0U; y < buffer.get_height(); ++y) {
        for (auto x = 0U; x < buffer.get_width(); ++x) {
            buffer[y][x] = png::rgba_pixel(0x00, 0x00, 0x00, 0xff);
        }
    }
}

bool rasterize::load_buffer_png() {
    std::ifstream ifs(BUFFER_PNG_NAME, std::ios::in | std::ios::binary);

    if (!ifs) {
        return false;
    }

    ifs.seekg(0, std::ios::end);

    auto size = ifs.tellg();

    ifs.seekg(0);

    on_memoey_png = new unsigned char[size];

    ifs.read(reinterpret_cast<char*>(on_memoey_png), size);

    on_memoey_png_size = static_cast<int>(size);

    return true;
}

bool rasterize::create_dx_handle() {
    if (on_memoey_png == nullptr || on_memoey_png_size <= 0) {
        return false;
    }

    if (handle == -1) {
        handle = CreateGraphFromMem(on_memoey_png, on_memoey_png_size);
    }
    else {
        handle = ReCreateGraphFromMem(on_memoey_png, on_memoey_png_size, handle);
    }

    return handle != -1;
}

void rasterize::render() {
    /*
    if (-1 == handle) {
        return;
    }
    */

    // TODO : TEST
    buffer.write(TEST_PNG_NAME);

    handle = LoadGraph(TEST_PNG_NAME);

    if (-1 == handle) {
        return;
    }

    DrawGraph(0, 0, handle, FALSE);
}
