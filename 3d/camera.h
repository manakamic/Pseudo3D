#pragma once

#include <memory>

namespace math {
    class vector3;
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
        bool culling(const math::vector3& position0, const math::vector3& position1, const math::vector3& position2) const;

        bool set_position(const math::vector3& position);
        bool set_target(const math::vector3& target);

        bool set_position(const double x, const double y, const double z);
        bool set_target(const double x, const double y, const double z);

        const std::shared_ptr<math::vector3> get_position() const { return position; }
        const std::shared_ptr<math::matrix44> get_matrix() const { return matrix; }

    private:
        std::shared_ptr<math::vector3> position;
        std::unique_ptr<math::vector3> target;
        std::unique_ptr<math::vector3> up;

        std::shared_ptr<math::matrix44> matrix;
    };

} // 3d
