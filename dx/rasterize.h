#pragma once
#include "png.hpp"

class rasterize {
public:
    static void Draw(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, png::image <png::rgba_pixel>& image);

    // �R���X�g���N�^
    rasterize();
    rasterize(const rasterize&) = default; // �R�s�[
    rasterize(rasterize&&) = default; // ���[�u

    // �f�X�g���N�^
    virtual ~rasterize();

    bool initialize(int width, int height);
    void render();

    void clear();

private:
    bool load_buffer_png();
    bool create_dx_handle();

    int handle;
};
