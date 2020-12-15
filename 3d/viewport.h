#pragma once

#include <memory>

namespace math {
    class matrix44;
}

namespace r3d {

    class viewport {
    public:
        // コンストラクタ
        viewport();
        viewport(const viewport&) = default; // コピー
        viewport(viewport&&) = default; // ムーブ

        // デストラクタ
        virtual ~viewport() = default;

        viewport& operator =(const viewport&) = default; // コピー
        viewport& operator =(viewport&&) = default; // ムーブ

        bool initialize(const double width, const double height);

        const  std::shared_ptr<math::matrix44> get_matrix() const { return matrix; }

    private:
        double width;
        double height;

        std::shared_ptr<math::matrix44> matrix;
    };
}
