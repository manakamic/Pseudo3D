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
    // DrawModiGraph �̑���ɃR�[������
    static void Draw(const std::array<std::shared_ptr<r3d::vertex>, 4>& vertices, const png::image <png::rgba_pixel>& image);

    // �R���X�g���N�^
    rasterize() = default;
    rasterize(const rasterize&) = default; // �R�s�[
    rasterize(rasterize&&) = default; // ���[�u

    // �f�X�g���N�^
    virtual ~rasterize();

    bool initialize(int width, int height);
    void clear();
    void render();
};
