#include <sstream>
#include <iostream>
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
    constexpr auto RGBA_BASE = 255.0;

    // rasterize::Draw が static メソッドなので 使用する変数類は static にする
    // png 処理用の変数は全て 2 つ用意して ダブルバッファを模して処理する
    png::image<png::rgb_pixel> buffer00;
    png::image<png::rgb_pixel> buffer01;

    std::thread db_thread00;
    std::thread db_thread01;

    int handle00 = -1;
    int handle01 = -1;

    bool double_buffer_flag = false;

    // デプスバッファ
    std::unique_ptr<double> depth_buffer = nullptr;
    int depth_buffer_size = 0;

#if defined(_USE_LIGHTING)
    std::unique_ptr<r3d::light> light_ptr = nullptr;

    bool create_light() {
        light_ptr.reset(new r3d::light);

        light_ptr->initialize();

        auto direction = math::vector4(-0.2, 0.2, 1.0);

        direction.normalized();

        light_ptr->set_direction(direction.normalize());

        return true;
    }
#endif

    void delete_thread(std::thread& th) {
        if (th.joinable()) {
            th.detach();
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

    // ダブルバッファ(png)を作成
    bool create_double_buffer(int width, int height) {
        png::image<png::rgb_pixel> image00(width, height);

        buffer00 = std::move(image00);

        png::image<png::rgb_pixel> image01(width, height);

        buffer01 = std::move(image01);

        return true;
    }

    // デプス バッファを作成
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
                buffer.set_pixel(x, y, png::rgb_pixel(0x00, 0x00, 0x00));
            }
        }
    }

    void clear_depth() {
        auto width = buffer00.get_width(); // png と同じサイズ
        auto height = buffer00.get_height();
        auto depth = depth_buffer.get();

        for (auto y = 0U; y < height; ++y) {
            for (auto x = 0U; x < width; ++x) {
                depth[height * y + x] = 1.0;
            }
        }
    }

    // 最小と最大の組み合わせを返す
    const std::tuple<int, int, int, int> get_min_max(const std::shared_ptr<math::vector4>& v0, const std::shared_ptr<math::vector4>& v1,
                                                     const std::shared_ptr<math::vector4>& v2, const std::shared_ptr<math::vector4>& v3) {
        auto x = { v0->get_x(),  v1->get_x(),  v2->get_x(),  v3->get_x() };
        auto y = { v0->get_y(),  v1->get_y(),  v2->get_y(),  v3->get_y() };

// DX ライブラリに同名の #define があるので undef する
#undef min
#undef max

        auto min_x = static_cast<int>(std::round(std::min(x)));
        auto min_y = static_cast<int>(std::round(std::min(y)));
        auto max_x = static_cast<int>(std::round(std::max(x)));
        auto max_y = static_cast<int>(std::round(std::max(y)));

        return std::make_tuple(min_x, min_y, max_x, max_y);
    }

    void clamp(double& value) {
        if (value < 0.0) {
            value = 0.0;
        }

        if (value > 1.0) {
            value = 1.0;
        }
    }

    // 3 頂点内のポイントの uv 値を 面積比率で補間
    // z 値も同様の補間が必要なので一緒に計算をする
    // さらに uv 値はパースペクティブ テクスチャ マッピング処理になる様に計算
#if !defined(_USE_LIGHTING)
    const std::tuple<double, double, double>
#else
#if defined(_USE_NORMAL_MAP)
    const std::tuple<double, double, double, math::vector4, math::vector4, math::vector4, math::vector4, image::color, image::color, double>
#else
    const std::tuple<double, double, double, math::vector4, math::vector4, image::color, image::color, double>
#endif
#endif
        get_perspective_uvz(const std::shared_ptr<r3d::vertex>& v0,
                            const std::shared_ptr<r3d::vertex>& v1,
                            const std::shared_ptr<r3d::vertex>& v2,
                            const math::vector4& point) {
        // 各引数の各値をローカル変数に取る
        auto p0 = v0->get_position(); auto p1 = v1->get_position(); auto p2 = v2->get_position();
        auto p0_x = p0->get_x(); auto p0_y = p0->get_y(); auto p0_z = p0->get_z(); auto p0_w = p0->get_w();
        auto p1_x = p1->get_x(); auto p1_y = p1->get_y(); auto p1_z = p1->get_z(); auto p1_w = p1->get_w();
        auto p2_x = p2->get_x(); auto p2_y = p2->get_y(); auto p2_z = p2->get_z(); auto p2_w = p2->get_w();
        auto p_x = point.get_x(); auto p_y = point.get_y();
        // 3頂点の辺とポイントで形成される 各3角形の面積割合で uv 値を補間する
        auto s0 = 0.5 * ((p1_x - p0_x) * (p2_y - p0_y) - (p1_y - p0_y) * (p2_x - p0_x));
        auto s1 = 0.5 * ((p2_x -  p_x) * (p0_y -  p_y) - (p2_y -  p_y) * (p0_x -  p_x));
        auto s2 = 0.5 * ((p0_x -  p_x) * (p1_y -  p_y) - (p0_y -  p_y) * (p1_x -  p_x));
        auto rate_1 = s1 / s0;
        auto rate_2 = s2 / s0;
        auto rate_0 = 1.0 - rate_1 - rate_2;
        // w 値も補間する
        auto rate_w = rate_0 / p0_w + rate_1 / p1_w + rate_2 / p2_w;
        auto uv0 = v0->get_uv();
        auto uv1 = v1->get_uv();
        auto uv2 = v2->get_uv();
        // 面積割割合で求めた uv を w 値で除算(パースペクティブ テクスチャマッピング用)
        auto uv0_u = rate_0 * uv0[0] / p0_w;
        auto uv0_v = rate_0 * uv0[1] / p0_w;
        auto uv1_u = rate_1 * uv1[0] / p1_w;
        auto uv1_v = rate_1 * uv1[1] / p1_w;
        auto uv2_u = rate_2 * uv2[0] / p2_w;
        auto uv2_v = rate_2 * uv2[1] / p2_w;
        // 最後に補間した w 値で除算すれば パースペクティブ テクスチャマッピング処理となる
        auto u = (uv0_u + uv1_u + uv2_u) / rate_w;
        auto v = (uv0_v + uv1_v + uv2_v) / rate_w;
        // uv を求める面積割合で z 値も補間
        auto z = rate_0 * p0_z + rate_1 * p1_z + rate_2 * p2_z;

        clamp(u);
        clamp(v);
        clamp(z);

#if !defined(_USE_LIGHTING)
        return std::make_tuple(u, v, z);
#else
        auto wp0 = v0->get_world_position(); auto wp1 = v1->get_world_position(); auto wp2 = v2->get_world_position();
        auto position = (*wp0 * rate_0) + (*wp1 * rate_1) + (*wp2 * rate_2);

        auto n0 = v0->get_normal(); auto n1 = v1->get_normal(); auto n2 = v2->get_normal();
        auto normal = (*n0 * rate_0) + (*n1 * rate_1) + (*n2 * rate_2);

        normal.normalized();

#if defined(_USE_NORMAL_MAP)
        auto t0 = v0->get_tangent(); auto t1 = v1->get_tangent(); auto t2 = v2->get_tangent();
        auto tangent = (*t0 * rate_0) + (*t1 * rate_1) + (*t2 * rate_2);

        tangent.normalized();

        auto b0 = v0->get_binormal(); auto b1 = v1->get_binormal(); auto b2 = v2->get_binormal();
        auto binormal = (*b0 * rate_0) + (*b1 * rate_1) + (*b2 * rate_2);

        binormal.normalized();
#endif

        auto d0 = v0->get_diffuse(); auto d1 = v1->get_diffuse(); auto d2 = v2->get_diffuse();
        auto diffuse = (*d0 * rate_0) + (*d1 * rate_1) + (*d2 * rate_2);

        auto sp0 = v0->get_speculer(); auto sp1 = v1->get_speculer(); auto sp2 = v2->get_speculer();
        auto speculer = (*sp0 * rate_0) + (*sp1 * rate_1) + (*sp2 * rate_2);

        auto pow0 = v0->get_speculer_power(); auto pow1 = v1->get_speculer_power(); auto pow2 = v2->get_speculer_power();
        auto speculer_power = (pow0 * rate_0) + (pow1 * rate_1) + (pow2 * rate_2);

#if defined(_USE_NORMAL_MAP)
        return std::make_tuple(u, v, z, position, normal, tangent, binormal, diffuse, speculer, speculer_power);
#else
        return std::make_tuple(u, v, z, position, normal, diffuse, speculer, speculer_power);
#endif
#endif
    }

    // uv 値から指定画像のピクセル値を取得
    const png::rgba_pixel get_rgba_with_uv(const double u, const double v, const png::image <png::rgba_pixel>& image) {
        auto rate_u = u / 1.0;
        auto rate_v = v / 1.0;
        auto width = static_cast<double>(image.get_width());
        auto height = static_cast<double>(image.get_height());
        auto x = static_cast<int>(width * rate_u);
        auto y = static_cast<int>(height * rate_v);

        return image.get_pixel(x, y);
    }

#if defined(_USE_LIGHTING)
#if defined(_USE_NORMAL_MAP)
    const png::rgba_pixel lighting(const std::shared_ptr<r3d::camera>& camera, const std::unique_ptr<r3d::light>& light_ptr,
                                   const std::shared_ptr<math::vector4> position, const std::shared_ptr<math::vector4> normal,
                                   const std::shared_ptr<math::vector4> tangent, const std::shared_ptr<math::vector4> binormal,
                                   const png::rgba_pixel& src, const png::rgba_pixel& src_normal, const std::shared_ptr<image::color> diffuse,
                                   const std::shared_ptr<image::color> speculer, const double speculer_power) {
        // ノーマルマップは接空間での法線情報
        auto src_normal_x = static_cast<double>(src_normal.red)   / RGBA_BASE * 2.0 - 1.0;
        auto src_normal_y = static_cast<double>(src_normal.green) / RGBA_BASE * 2.0 - 1.0;
        auto src_normal_z = static_cast<double>(src_normal.blue)  / RGBA_BASE * 2.0 - 1.0;
        auto normal_map_normal = math::vector4(src_normal_x, src_normal_y, src_normal_z);

        normal_map_normal.normalized();
#else
    const png::rgba_pixel lighting(const std::shared_ptr<r3d::camera>& camera, const std::unique_ptr<r3d::light>& light_ptr,
                                   const std::shared_ptr<math::vector4> position, const std::shared_ptr<math::vector4> normal,
                                   const png::rgba_pixel& src, const std::shared_ptr<image::color> diffuse,
                                   const std::shared_ptr<image::color> speculer, const double speculer_power) {
#endif
        auto src_r = static_cast<double>(src.red);
        auto src_g = static_cast<double>(src.green);
        auto src_b = static_cast<double>(src.blue);
        // ライトベクトル
        auto light_dir = light_ptr->get_direction();
        
#if defined(_USE_NORMAL_MAP)
        // ライトを接空間へ変換
        auto tangent_light_x = light_dir->dot(*tangent);
        auto tangent_light_y = light_dir->dot(*binormal);
        auto tangent_light_z = light_dir->dot(*normal);
        // 反転
        auto tangent_light = math::vector4(-tangent_light_x, -tangent_light_y, -tangent_light_z);

        tangent_light.normalized();

        auto diffuse_dot = tangent_light.dot(normal_map_normal);
#else
        // 反転
        auto light = math::vector4(-light_dir->get_x(), -light_dir->get_y(), -light_dir->get_z());
        // ライトと法線の内積
        auto diffuse_dot = light.dot(*normal);
#endif

        if (diffuse_dot < 0.0) {
            diffuse_dot = 0.0;
        }
        else if (diffuse_dot > 1.0) {
            diffuse_dot = 1.0;
        }

        // 擬似拡散反射
        auto diffuse_r = static_cast<double>(diffuse->get_r()) / RGBA_BASE;
        auto diffuse_g = static_cast<double>(diffuse->get_g()) / RGBA_BASE;
        auto diffuse_b = static_cast<double>(diffuse->get_b()) / RGBA_BASE;
        auto dr = diffuse_r * diffuse_dot;
        auto dg = diffuse_g * diffuse_dot;
        auto db = diffuse_b * diffuse_dot;

        src_r *= dr;
        src_g *= dg;
        src_b *= db;

        // 擬似鏡面反射(ハーフベクトル)
        auto speculer_r = static_cast<double>(speculer->get_r());
        auto speculer_g = static_cast<double>(speculer->get_g());
        auto speculer_b = static_cast<double>(speculer->get_b());
#if true
        // 視線ベクトル
        auto camera_position = camera->get_position();

#if defined(_USE_NORMAL_MAP)
        auto eye = math::vector4(position->get_x() - camera_position->get_x(),
                                 position->get_y() - camera_position->get_y(),
                                 position->get_z() - camera_position->get_z());

        eye.normalized();

        // 視線を接空間へ変換
        auto tangent_eye_x = eye.dot(*tangent);
        auto tangent_eye_y = eye.dot(*binormal);
        auto tangent_eye_z = eye.dot(*normal);
        // 反転
        auto tangent_eye = math::vector4(-tangent_eye_x, -tangent_eye_y, -tangent_eye_z);

        tangent_eye.normalized();

        // 視線とライトのハーフベクトル
        auto half = math::vector4(tangent_eye.get_x() + tangent_light.get_x(),
                                  tangent_eye.get_y() + tangent_light.get_y(),
                                  tangent_eye.get_z() + tangent_light.get_z());

        half.normalized();

        auto speculer_dot = half.dot(normal_map_normal);
#else
        auto eye = math::vector4(camera_position->get_x() - position->get_x(),
                                 camera_position->get_y() - position->get_y(),
                                 camera_position->get_z() - position->get_z());

        eye.normalized();

        // 視線とライトのハーフベクトル
        auto half = math::vector4(eye.get_x() + light.get_x(), eye.get_y() + light.get_y(), eye.get_z() + light.get_z());

        half.normalized();

        auto speculer_dot = half.dot(*normal);
#endif

        if (speculer_dot < 0.0) {
            speculer_dot = 0.0;
        } else if (speculer_dot > 1.0) {
            speculer_dot = 1.0;
        }

        auto speculer_pow = std::pow(speculer_dot, speculer_power);

        if (dr > 0) {
            src_r += speculer_r * speculer_pow;
        }

        if (dg > 0) {
            src_g += speculer_g * speculer_pow;
        }

        if (db > 0) {
            src_b += speculer_b * speculer_pow;
        }
#endif

        if (src_r > RGBA_BASE) {
            src_r = RGBA_BASE;
        }

        if (src_g > RGBA_BASE) {
            src_g = RGBA_BASE;
        }

        if (src_b > RGBA_BASE) {
            src_b = RGBA_BASE;
        }

        auto r = static_cast<png::byte>(std::round(src_r));
        auto g = static_cast<png::byte>(std::round(src_g));
        auto b = static_cast<png::byte>(std::round(src_b));

        return png::rgba_pixel(r, g, b, src.alpha);
    }
#endif

    // アルファブレンドでピクセルを書き込む
    bool alpha_blend(const int x, const int y, const png::rgba_pixel& src, png::image<png::rgb_pixel>& buffer) {
        if (src.alpha > 0x00) {
            auto src_a = static_cast<double>(src.alpha) / RGBA_BASE;
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

            // ポイントサンプリング
            buffer.set_pixel(x, y, png::rgb_pixel(r, g, b));

            return true;
        }

        return false;
    }

    // スレッドで行う処理
    void process_thread(png::image<png::rgb_pixel>& buffer, int& handle) {
        std::ostringstream stream;

        buffer.write_stream(stream); // ストリーム書き出し
        clear_buffer(buffer);        // ストリーム書き込みが終わったらクリアしておく

        auto str = stream.str();

        if (handle == -1) {
            handle = CreateGraphFromMem(str.c_str(), str.size());
        }
        else {
            ReCreateGraphFromMem(str.c_str(), str.size(), handle);
        }
    }
}

#if defined(_USE_LIGHTING)
#if defined(_USE_NORMAL_MAP)
void rasterize::Draw(const std::array<std::shared_ptr<r3d::vertex>, 4>& vertices, const png::image <png::rgba_pixel>& image,
                     const png::image <png::rgba_pixel>& image_normal, const std::shared_ptr<r3d::camera>& camera) {
#else
void rasterize::Draw(const std::array<std::shared_ptr<r3d::vertex>, 4>& vertices, const png::image <png::rgba_pixel>& image,
                     const std::shared_ptr<r3d::camera>& camera) {
#endif
#else
void rasterize::Draw(const std::array<std::shared_ptr<r3d::vertex>, 4>&vertices, const png::image <png::rgba_pixel>&image) {
#endif
    auto width = static_cast<int>(buffer00.get_width());
    auto height = static_cast<int>(buffer00.get_height());

    if (width <= 0 || height <= 0) {
        return;
    }

    // 指定の矩形で、同一平面上の 3 角形を 2 つ作る
    auto p0 = vertices[0]->get_position();
    auto p1 = vertices[1]->get_position();
    auto p2 = vertices[2]->get_position();
    auto p3 = vertices[3]->get_position();

    auto v0 = math::vector4(p0->get_x(), p0->get_y(), 0.0f);
    auto v1 = math::vector4(p1->get_x(), p1->get_y(), 0.0f);
    auto v2 = math::vector4(p2->get_x(), p2->get_y(), 0.0f);
    auto v3 = math::vector4(p3->get_x(), p3->get_y(), 0.0f);

    // 4 頂点を含む最小の矩形でループさせる
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
#if defined(_USE_NORMAL_MAP)
            std::shared_ptr<math::vector4> tangent = nullptr;
            std::shared_ptr<math::vector4> binormal = nullptr;
#endif
            std::shared_ptr<image::color> diffuse = nullptr;
            std::shared_ptr<image::color> speculer = nullptr;
            auto speculer_power = 1.0;
#endif

            // 各 3 角形の内側のピクセルなら処理する
            if (math::utility::inside_triangle_point(v0, v1, v2, p)) {
                auto uvz = get_perspective_uvz(vertices[0], vertices[1], vertices[2], p);

                u = std::get<0>(uvz); v = std::get<1>(uvz); z = std::get<2>(uvz);
                in = true;

#if defined(_USE_LIGHTING)
                position.reset(new math::vector4(std::get<3>(uvz)));
                normal.reset(new math::vector4(std::get<4>(uvz)));
#if defined(_USE_NORMAL_MAP)
                tangent.reset(new math::vector4(std::get<5>(uvz)));
                binormal.reset(new math::vector4(std::get<6>(uvz)));
                diffuse.reset(new image::color(std::get<7>(uvz)));
                speculer.reset(new image::color(std::get<8>(uvz)));
                speculer_power = std::get<9>(uvz);
#else
                diffuse.reset(new image::color(std::get<5>(uvz)));
                speculer.reset(new image::color(std::get<6>(uvz)));
                speculer_power = std::get<7>(uvz);
#endif
#endif
            }
            else if (math::utility::inside_triangle_point(v1, v3, v2, p)) {
                auto uvz = get_perspective_uvz(vertices[1], vertices[3], vertices[2], p);

                u = std::get<0>(uvz); v = std::get<1>(uvz); z = std::get<2>(uvz);
                in = true;

#if defined(_USE_LIGHTING)
                position.reset(new math::vector4(std::get<3>(uvz)));
                normal.reset(new math::vector4(std::get<4>(uvz)));
#if defined(_USE_NORMAL_MAP)
                tangent.reset(new math::vector4(std::get<5>(uvz)));
                binormal.reset(new math::vector4(std::get<6>(uvz)));
                diffuse.reset(new image::color(std::get<7>(uvz)));
                speculer.reset(new image::color(std::get<8>(uvz)));
                speculer_power = std::get<9>(uvz);
#else
                diffuse.reset(new image::color(std::get<5>(uvz)));
                speculer.reset(new image::color(std::get<6>(uvz)));
                speculer_power = std::get<7>(uvz);
#endif
#endif
            }

            if (in && depth[depth_pos] > z) { // Z バッファ処理
#if defined(_USE_LIGHTING)
                auto src = get_rgba_with_uv(u, v, image);
#if defined(_USE_NORMAL_MAP)
                auto src_normal = get_rgba_with_uv(u, v, image_normal);
                // ライティング処理
                auto pixel = lighting(camera, light_ptr, position, normal, tangent, binormal,
                                      src, src_normal, diffuse, speculer, speculer_power);
#else
                // ライティング処理
                auto pixel = lighting(camera, light_ptr, position, normal, src, diffuse, speculer, speculer_power);
#endif
#else
                auto pixel = get_rgba_with_uv(u, v, image);
#endif

                // アルファブレンド
                if (alpha_blend(x, y, pixel, double_buffer_flag ? buffer01 : buffer00)) {
                    depth[depth_pos] = z; // ピクセルを書き込んだら デプスバッファを更新
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

int rasterize::double_buffering() {
    auto handle = 0;

    if (double_buffer_flag) {
        // スレッドで処理された png をロード
        std::thread th01([]() {
            process_thread(buffer01, handle01);
        });

        // スレッド処理を変数に保存
        db_thread01 = std::move(th01);

        // 前フレームで作成したスレッドの同期
        if (db_thread00.joinable()) {
            db_thread00.join();
        }

        // 同期処理が終わったハンドルを使用する
        handle = handle00;
        // バッファ切り替え
        double_buffer_flag = false;
    } else {
        std::thread th00([]() {
            process_thread(buffer00, handle00);
        });

        db_thread00 = std::move(th00);

        if (db_thread01.joinable()) {
            db_thread01.join();
        }

        handle = handle01;
        double_buffer_flag = true;
    }

    return handle;
}

void rasterize::render() {
    auto handle = double_buffering();

    if (handle == -1) {
        return;
    }

    DrawGraph(0, 0, handle, FALSE);
}
