#include <memory>
#include <fstream>
#include "DxLib.h"
#include "rasterize.h"

namespace {
    constexpr auto BUFFER_PNG_NAME = _T("buffer.png");

    png::image<png::rgb_pixel> buffer;

    unsigned char* on_memoey_png = nullptr;
    auto on_memoey_png_size = 0;
}

void rasterize::Draw(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, png::image <png::rgba_pixel>& image) {
}

rasterize::rasterize() {
    width = 0;
    height = 0;
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

    this->width = width;
    this->height = height;

    png::image<png::rgb_pixel> image(width, height);

    buffer = std::move(image);

    return true;
}

void rasterize::clear() {
    for (auto i = 0U; i < buffer.get_height(); ++i) {
        for (auto j = 0U; j < buffer.get_width(); ++j) {
            buffer[i][j] = png::rgb_pixel(0x00, 0x00, 0x00);
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
    if (-1 == handle) {
        return;
    }

    DrawGraph(0, 0, handle, FALSE);
}
