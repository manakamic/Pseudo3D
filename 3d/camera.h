#pragma once

#include <memory>

namespace math {
    class vector4;
    class matrix44;
}

namespace r3d {

    class camera {
    public:
        // コンストラクタ
        camera();
        camera(const camera&) = default; // コピー
        camera(camera&&) = default; // ムーブ

        // デストラクタ
        virtual ~camera() = default;

        camera& operator =(const camera&) = default; // コピー
        camera& operator =(camera&&) = default; // ムーブ

        bool initialize();
        void look_at();
        bool culling(const math::vector4& position0, const math::vector4& position1, const math::vector4& position2) const;

        bool set_position(const math::vector4& position);
        bool set_target(const math::vector4& target);

        bool set_position(const double x, const double y, const double z);
        bool set_target(const double x, const double y, const double z);

        math::vector4 get_direction() const;

        const std::unique_ptr<math::vector4>& get_target() const { return target; }

        const std::shared_ptr<math::vector4> get_position() const { return position; }
        const std::shared_ptr<math::matrix44> get_matrix() const { return matrix; }

    private:
        std::shared_ptr<math::vector4> position;
        std::unique_ptr<math::vector4> target;
        std::unique_ptr<math::vector4> up;

        std::shared_ptr<math::matrix44> matrix;
    };

} // 3d
