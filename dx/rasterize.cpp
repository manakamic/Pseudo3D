#include <fstream>
#include <cmath>
#include <tuple>
#include <algorithm>
#include "vector4.h"
#include "vertex.h"
#include "utility.h"
#include "DxLib.h"
#include "rasterize.h"
#if defined(_USE_LIGHTING)
#include "camera.h"
#include "color.h"
#include "light.h"
#endif

namespace {
    constexpr auto BUFFER_00_PNG = _T("buffer00.png");
    constexpr auto BUFFER_01_PNG = _T("buffer01.png");
    constexpr auto ALPHA_BASE = 255.0;

    // rasterize::Draw �� static ���\�b�h�Ȃ̂� �g�p����ϐ��ނ� static �ɂ���
    // png �����p�̕ϐ��͑S�� 2 �p�ӂ��� �_�u���o�b�t�@��͂��ď�������
    png::image<png::rgb_pixel> buffer00;
    png::image<png::rgb_pixel> buffer01;

    std::thread db_thread00;
    std::thread db_thread01;

    int handle00 = -1;
    int handle01 = -1;

    bool double_buffer_flag = false;

    // �f�v�X�o�b�t�@
    std::unique_ptr<double> depth_buffer = nullptr;
    int depth_buffer_size = 0;

#if defined(_USE_LIGHTING)
    std::unique_ptr<r3d::light> light_ptr = nullptr;

    bool create_light() {
        light_ptr.reset(new r3d::light);

        light_ptr->initialize();

        auto direction = math::vector4(0.0, 0.0, 1.0);

        direction.normalized();

        light_ptr->set_direction(direction.normalize());

        return true;
    }
#endif

    void delete_thread(std::thread& th) {
        if (th.joinable()) {
            th.join();
        }
    }

    void delete_thread() {
        delete_thread(db_thread01);
        delete_thread(db_thread00);
    }

    void delete_handle(int& handle) {
        if (handle != -1) {
            DeleteGraph(handle);
            handle = -1;
        }
    }

    void delete_handle() {
        delete_handle(handle01);
        delete_handle(handle00);
    }

    // �_�u���o�b�t�@(png)���쐬
    bool create_double_buffer(int width, int height) {
        png::image<png::rgb_pixel> image00(width, height);

        buffer00 = std::move(image00);

        png::image<png::rgb_pixel> image01(width, height);

        buffer01 = std::move(image01);

        return true;
    }

    // �f�v�X �o�b�t�@���쐬
    bool create_depth_buffer(int width, int height) {
        depth_buffer_size = width * height;

        auto buffer = new double[depth_buffer_size] { 0.0 };

        depth_buffer.reset(buffer);

        return true;
    }

    void clear_buffer(png::image<png::rgb_pixel>& buffer) {
        auto width = buffer.get_width();
        auto height = buffer.get_height();

        for (auto y = 0U; y < height; ++y) {
            for (auto x = 0U; x < width; ++x) {
                buffer[y][x] = png::rgb_pixel(0x00, 0x00, 0x00);
            }
        }
    }

    void clear_depth() {
        auto width = buffer00.get_width(); // png �Ɠ����T�C�Y
        auto height = buffer00.get_height();
        auto depth = depth_buffer.get();

        for (auto y = 0U; y < height; ++y) {
            for (auto x = 0U; x < width; ++x) {
                depth[height * y + x] = 1.0;
            }
        }
    }

    // �ŏ��ƍő�̑g�ݍ��킹��Ԃ�
    std::tuple<int, int, int, int> get_min_max(const std::shared_ptr<math::vector4>& v0, const std::shared_ptr<math::vector4>& v1,
                                               const std::shared_ptr<math::vector4>& v2, const std::shared_ptr<math::vector4>& v3) {
        auto x = { v0->get_x(),  v1->get_x(),  v2->get_x(),  v3->get_x() };
        auto y = { v0->get_y(),  v1->get_y(),  v2->get_y(),  v3->get_y() };

// DX ���C�u�����ɓ����� #define ������̂� undef ����
#undef min
#undef max

        auto min_x = static_cast<int>(std::round(std::min(x)));
        auto min_y = static_cast<int>(std::round(std::min(y)));
        auto max_x = static_cast<int>(std::round(std::max(x)));
        auto max_y = static_cast<int>(std::round(std::max(y)));

        return std::make_tuple(min_x, min_y, max_x, max_y);
    }

    // 3 ���_���̃|�C���g�� uv �l�� �ʐϔ䗦�ŕ��
    // z �l�����l�̕�Ԃ��K�v�Ȃ̂ňꏏ�Ɍv�Z������
    // ����� uv �l�̓p�[�X�y�N�e�B�u �e�N�X�`�� �}�b�s���O�����ɂȂ�l�Ɍv�Z
#if !defined(_USE_LIGHTING)
    std::tuple<double, double, double>
#else
    std::tuple<double, double, double, math::vector4, math::vector4, image::color, image::color, double>
#endif
        get_perspective_uvz(const std::shared_ptr<r3d::vertex>& v0,
                            const std::shared_ptr<r3d::vertex>& v1,
                            const std::shared_ptr<r3d::vertex>& v2,
                            const math::vector4& point) {
        // �e�����̊e�l�����[�J���ϐ��Ɏ��
        auto p0 = v0->get_position(); auto p1 = v1->get_position(); auto p2 = v2->get_position();
        auto p0_x = p0->get_x(); auto p0_y = p0->get_y(); auto p0_z = p0->get_z(); auto p0_w = p0->get_w();
        auto p1_x = p1->get_x(); auto p1_y = p1->get_y(); auto p1_z = p1->get_z(); auto p1_w = p1->get_w();
        auto p2_x = p2->get_x(); auto p2_y = p2->get_y(); auto p2_z = p2->get_z(); auto p2_w = p2->get_w();
        auto p_x = point.get_x(); auto p_y = point.get_y();
        // 3���_�̕ӂƃ|�C���g�Ō`������� �e3�p�`�̖ʐϊ����� uv �l���Ԃ���
        auto s0 = 0.5 * ((p1_x - p0_x) * (p2_y - p0_y) - (p1_y - p0_y) * (p2_x - p0_x));
        auto s1 = 0.5 * ((p2_x -  p_x) * (p0_y -  p_y) - (p2_y -  p_y) * (p0_x -  p_x));
        auto s2 = 0.5 * ((p0_x -  p_x) * (p1_y -  p_y) - (p0_y -  p_y) * (p1_x -  p_x));
        auto rate_1 = s1 / s0;
        auto rate_2 = s2 / s0;
        auto rate_0 = 1.0 - rate_1 - rate_2;
        // w �l����Ԃ���
        auto rate_w = rate_0 / p0_w + rate_1 / p1_w + rate_2 / p2_w;
        auto uv0 = v0->get_uv();
        auto uv1 = v1->get_uv();
        auto uv2 = v2->get_uv();
        // �ʐϊ������ŋ��߂� uv �� w �l�ŏ��Z(�p�[�X�y�N�e�B�u �e�N�X�`���}�b�s���O�p)
        auto uv0_u = rate_0 * uv0[0] / p0_w;
        auto uv0_v = rate_0 * uv0[1] / p0_w;
        auto uv1_u = rate_1 * uv1[0] / p1_w;
        auto uv1_v = rate_1 * uv1[1] / p1_w;
        auto uv2_u = rate_2 * uv2[0] / p2_w;
        auto uv2_v = rate_2 * uv2[1] / p2_w;
        // �Ō�ɕ�Ԃ��� w �l�ŏ��Z����� �p�[�X�y�N�e�B�u �e�N�X�`���}�b�s���O�����ƂȂ�
        auto u = (uv0_u + uv1_u + uv2_u) / rate_w;
        auto v = (uv0_v + uv1_v + uv2_v) / rate_w;
        // uv �����߂�ʐϊ����� z �l�����
        auto z = rate_0 * p0_z + rate_1 * p1_z + rate_2 * p2_z;

#if !defined(_USE_LIGHTING)
        return std::make_tuple(u, v, z);
#else
        auto position = (*p0 * rate_0) + (*p1 * rate_1) + (*p2 * rate_2);

        auto n0 = v0->get_normal(); auto n1 = v1->get_normal(); auto n2 = v2->get_normal();
        auto normal = (*n0 * rate_0) + (*n1 * rate_1) + (*n2 * rate_2);

        normal.normalized();

        auto d0 = v0->get_diffuse(); auto d1 = v1->get_diffuse(); auto d2 = v2->get_diffuse();
        auto diffuse = (*d0 * rate_0) + (*d1 * rate_1) + (*d2 * rate_2);

        auto sp0 = v0->get_speculer(); auto sp1 = v1->get_speculer(); auto sp2 = v2->get_speculer();
        auto speculer = (*sp0 * rate_0) + (*sp1 * rate_1) + (*sp2 * rate_2);

        auto pow0 = v0->get_speculer_power(); auto pow1 = v1->get_speculer_power(); auto pow2 = v2->get_speculer_power();
        auto speculer_power = (pow0 * rate_0) + (pow1 * rate_1) + (pow2 * rate_2);

        return std::make_tuple(u, v, z, position, normal, diffuse, speculer, speculer_power);
#endif
    }

    // uv �l����w��摜�̃s�N�Z���l���擾
    png::rgba_pixel get_rgba_with_uv(const double u, const double v, const png::image <png::rgba_pixel>& image) {
        auto rate_u = u / 1.0;
        auto rate_v = v / 1.0;
        auto width = static_cast<double>(image.get_width());
        auto height = static_cast<double>(image.get_height());
        auto x = static_cast<int>(width * rate_u);
        auto y = static_cast<int>(height * rate_v);

        return image[y][x];
    }

#if defined(_USE_LIGHTING)
    png::rgba_pixel lighting(const std::shared_ptr<r3d::camera>& camera, const std::unique_ptr<r3d::light>& light_ptr,
                             const std::shared_ptr<math::vector4> position, const std::shared_ptr<math::vector4> normal,
                             const png::rgba_pixel& src, std::shared_ptr<image::color> diffuse, std::shared_ptr<image::color> speculer, double speculer_power) {
        auto src_r = static_cast<double>(src.red);
        auto src_g = static_cast<double>(src.green);
        auto src_b = static_cast<double>(src.blue);
        // ���C�g�x�N�g��(�\�ߔ��]��)
        auto light = light_ptr->get_direction()->normalize();
        auto normal_dir = normal->normalize();
        // ���C�g�Ɩ@���̓���
        auto diffuse_dot = light.dot(normal_dir);

        if (diffuse_dot < 0.0) {
            diffuse_dot = 0.0;
        }
        else if (diffuse_dot > 1.0) {
            diffuse_dot = 1.0;
        }

        // �[���g�U����
        auto diffuse_r = static_cast<double>(diffuse->get_r()) / 255.0;
        auto diffuse_g = static_cast<double>(diffuse->get_g()) / 255.0;
        auto diffuse_b = static_cast<double>(diffuse->get_b()) / 255.0;
        auto dr = diffuse_r * diffuse_dot;
        auto dg = diffuse_g * diffuse_dot;
        auto db = diffuse_b * diffuse_dot;

        // �[�����ʔ���(�n�[�t�x�N�g��)
        auto speculer_r = static_cast<double>(speculer->get_r()) / 255.0;
        auto speculer_g = static_cast<double>(speculer->get_g()) / 255.0;
        auto speculer_b = static_cast<double>(speculer->get_b()) / 255.0;
#if false
        // �����x�N�g��
        auto camera_position = camera->get_position();
        auto eye = math::vector4(camera_position->get_x() - position->get_x(), camera_position->get_y() - position->get_y(), camera_position->get_z() - position->get_z());

        eye.normalized();

        // �����ƃ��C�g�̃n�[�t�x�N�g��
        auto half = math::vector4(eye.get_x() + light.get_x(), eye.get_y() + light.get_y(), eye.get_z() + light.get_z());

        half.normalized();

        auto speculer_dot = half.dot(normal_dir);

        if (speculer_dot < 0.0) {
            speculer_dot = 0.0;
        } else if (speculer_dot > 1.0) {
            speculer_dot = 1.0;
        }

        auto speculer_pow = std::pow(speculer_dot, speculer_power);
#else
        auto speculer_pow = 0.0;
#endif

        if (dr > 0) {
            dr += speculer_r * speculer_pow;
        }

        if (dg > 0) {
            dg += speculer_g * speculer_pow;
        }

        if (db > 0) {
            db += speculer_b * speculer_pow;
        }

        src_r *= dr;
        src_g *= dg;
        src_b *= db;

        if (src_r > 255.0) {
            src_r = 255.0;
        }

        if (src_g > 255.0) {
            src_g = 255.0;
        }

        if (src_b > 255.0) {
            src_b = 255.0;
        }

        auto r = static_cast<png::byte>(std::round(src_r));
        auto g = static_cast<png::byte>(std::round(src_g));
        auto b = static_cast<png::byte>(std::round(src_b));

        return png::rgba_pixel(r, g, b, src.alpha);
    }
#endif

    // �A���t�@�u�����h�Ńs�N�Z������������
    bool alpha_blend(const int x, const int y, const png::rgba_pixel& src, png::image<png::rgb_pixel>& buffer) {
        if (src.alpha > 0x00) {
            auto src_a = static_cast<double>(src.alpha) / ALPHA_BASE;
            auto src_r = static_cast<png::byte>(src_a * static_cast<double>(src.red));
            auto src_g = static_cast<png::byte>(src_a * static_cast<double>(src.green));
            auto src_b = static_cast<png::byte>(src_a * static_cast<double>(src.blue));

            png::rgb_pixel dst = buffer[y][x];

            auto dst_a = 1.0 - src_a;
            auto dst_r = static_cast<png::byte>(dst_a * static_cast<double>(dst.red));
            auto dst_g = static_cast<png::byte>(dst_a * static_cast<double>(dst.green));
            auto dst_b = static_cast<png::byte>(dst_a * static_cast<double>(dst.blue));

            auto r = src_r + dst_r;
            auto g = src_g + dst_g;
            auto b = src_b + dst_b;

            // �|�C���g�T���v�����O
            buffer[y][x] = png::rgb_pixel(r, g, b);

            return true;
        }

        return false;
    }

    // �X���b�h(C++11 �� DX ���C�u����)�̓���
    void synchronized_thread(std::thread& th, int& handle) {
        if (th.joinable()) {
            th.join();

            while (CheckHandleASyncLoad(handle) != FALSE) {
                ProcessMessage(); // ProcessMessage �Ŕ񓯊����[�h���i��

                // �������[�v�Ȃ̂Ń��C�����[�v�Ɠ����E�o�������L�q
                if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
                    handle = -1;
                    break;
                }
            }

            SetUseASyncLoadFlag(FALSE); // �񓯊����[�h�𖳌��ɂ���
        }
    }

    // �X���b�h�ōs������
    void process_thread(const TCHAR* png_name, png::image<png::rgb_pixel>& buffer, int& handle) {
        buffer.write(png_name);       // �t�@�C�������o��
        clear_buffer(buffer);         // �t�@�C���������݂��I�������N���A���Ă���
        SetUseASyncLoadFlag(TRUE);    // �񓯊����[�h��L���ɂ���
        delete_handle(handle);        // �O��̃��[�h�摜��j��(���[�h���J��Ԃ��̂ł�����Ɣj������)
        handle = LoadGraph(png_name); // �񓯊����[�h
    }
}

#if defined(_USE_LIGHTING)
void rasterize::Draw(const std::array<std::shared_ptr<r3d::vertex>, 4>& vertices, const png::image <png::rgba_pixel>& image, const std::shared_ptr<r3d::camera>& camera) {
#else
void rasterize::Draw(const std::array<std::shared_ptr<r3d::vertex>, 4>&vertices, const png::image <png::rgba_pixel>&image) {
#endif
    auto width = static_cast<int>(buffer00.get_width());
    auto height = static_cast<int>(buffer00.get_height());

    if (width <= 0 || height <= 0) {
        return;
    }

    // �w��̋�`�ŁA���ꕽ�ʏ�� 3 �p�`�� 2 ���
    auto p0 = vertices[0]->get_position();
    auto p1 = vertices[1]->get_position();
    auto p2 = vertices[2]->get_position();
    auto p3 = vertices[3]->get_position();

    auto v0 = math::vector4(p0->get_x(), p0->get_y(), 0.0f);
    auto v1 = math::vector4(p1->get_x(), p1->get_y(), 0.0f);
    auto v2 = math::vector4(p2->get_x(), p2->get_y(), 0.0f);
    auto v3 = math::vector4(p3->get_x(), p3->get_y(), 0.0f);

    // 4 ���_���܂ލŏ��̋�`�Ń��[�v������
    auto min_max = get_min_max(p0, p1, p2, p3);
    auto min_x = std::get<0>(min_max);
    auto min_y = std::get<1>(min_max);
    auto max_x = std::get<2>(min_max);
    auto max_y = std::get<3>(min_max);

    auto depth = depth_buffer.get();

    for (auto y = min_y; y <= max_y; ++y) {
        if (y < 0 || y >= height) {
            continue;
        }

        for (auto x = min_x; x <= max_x; ++x) {
            if (x < 0 || x >= width) {
                continue;
            }

            auto p = math::vector4(static_cast<double>(x), static_cast<double>(y), 0.0f);
            auto u = 0.0; auto v = 0.0; auto z = 0.0;
            auto depth_pos = y * height + x;
            auto in = false;
#if defined(_USE_LIGHTING)
            std::shared_ptr<math::vector4> position = nullptr;
            std::shared_ptr<math::vector4> normal = nullptr;
            std::shared_ptr<image::color> diffuse = nullptr;
            std::shared_ptr<image::color> speculer = nullptr;
            auto speculer_power = 1.0;
#endif

            // �e 3 �p�`�̓����̃s�N�Z���Ȃ珈������
            if (math::utility::inside_triangle_point(v0, v1, v2, p)) {
                auto uvz = get_perspective_uvz(vertices[0], vertices[1], vertices[2], p);

                u = std::get<0>(uvz); v = std::get<1>(uvz); z = std::get<2>(uvz);
                in = true;

#if defined(_USE_LIGHTING)
                position.reset(new math::vector4(std::get<3>(uvz)));
                normal.reset(new math::vector4(std::get<4>(uvz)));
                diffuse.reset(new image::color(std::get<5>(uvz)));
                speculer.reset(new image::color(std::get<6>(uvz)));
                speculer_power = std::get<7>(uvz);
#endif
            }
            else if (math::utility::inside_triangle_point(v1, v3, v2, p)) {
                auto uvz = get_perspective_uvz(vertices[1], vertices[3], vertices[2], p);

                u = std::get<0>(uvz); v = std::get<1>(uvz); z = std::get<2>(uvz);
                in = true;

#if defined(_USE_LIGHTING)
                position.reset(new math::vector4(std::get<3>(uvz)));
                normal.reset(new math::vector4(std::get<4>(uvz)));
                diffuse.reset(new image::color(std::get<5>(uvz)));
                speculer.reset(new image::color(std::get<6>(uvz)));
                speculer_power = std::get<7>(uvz);
#endif
            }

            if (in && depth[depth_pos] > z) { // Z �o�b�t�@����
#if defined(_USE_LIGHTING)
                auto src = get_rgba_with_uv(u, v, image);
                // ���C�e�B���O����
                auto pixel = lighting(camera, light_ptr, position, normal, src, diffuse, speculer, speculer_power);
#else
                auto pixel = get_rgba_with_uv(u, v, image);
#endif

                // �A���t�@�u�����h
                if (alpha_blend(x, y, pixel, double_buffer_flag ? buffer01 : buffer00)) {
                    depth[depth_pos] = z; // �s�N�Z�����������񂾂� �f�v�X�o�b�t�@���X�V
                }
            }
        }
    }
}

rasterize::~rasterize() {
    delete_thread();
    delete_handle();
}

bool rasterize::initialize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return false;
    }

    if (!create_double_buffer(width, height)) {
        return false;
    }

    if (!create_depth_buffer(width, height)) {
        return false;
    }

    std::remove(BUFFER_00_PNG);
    std::remove(BUFFER_01_PNG);

#if defined(_USE_LIGHTING)
    if (!create_light()) {
        return false;
    }
#endif

    return true;
}

void rasterize::clear() {
    clear_depth();
}

void rasterize::render() {
    if (double_buffer_flag) {
        // �O�t���[���ō쐬�����X���b�h�̓���
        synchronized_thread(db_thread00, handle00);

        // �X���b�h�ŏ������ꂽ png �����[�h
        std::thread th01([]() {
            process_thread(BUFFER_01_PNG, buffer01, handle01);
        });

        db_thread01 = std::move(th01);
    }
    else {
        synchronized_thread(db_thread01, handle01);

        std::thread th00([]() {
            process_thread(BUFFER_00_PNG, buffer00, handle00);
        });

        db_thread00 = std::move(th00);
    }

    // �Ǎ����I����Ă�������g�p����
    auto handle = double_buffer_flag ? handle00 : handle01;

    double_buffer_flag = !double_buffer_flag;

    if (handle == -1) {
        return;
    }

    DrawGraph(0, 0, handle, FALSE);
}
