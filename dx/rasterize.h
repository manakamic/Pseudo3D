#pragma once

#include <array>
#include <memory>
#include <thread>
#include "png.hpp"

namespace r3d {
    class vertex;
#if defined(_USE_LIGHTING)
    class camera;
#endif
}

class rasterize {
public:
    // DrawModiGraph �̑���ɃR�[������
#if defined(_USE_LIGHTING)
    static void Draw(const std::array<std::shared_ptr<r3d::vertex>, 4>& vertices, const png::image <png::rgba_pixel>& image,const std::shared_ptr<r3d::camera>& camera);
#else
    static void Draw(const std::array<std::shared_ptr<r3d::vertex>, 4>& vertices, const png::image <png::rgba_pixel>& image);
#endif

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
