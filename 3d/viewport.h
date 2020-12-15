#pragma once

#include <memory>

namespace math {
    class matrix44;
}

namespace r3d {

    class viewport {
    public:
        // �R���X�g���N�^
        viewport();
        viewport(const viewport&) = default; // �R�s�[
        viewport(viewport&&) = default; // ���[�u

        // �f�X�g���N�^
        virtual ~viewport() = default;

        viewport& operator =(const viewport&) = default; // �R�s�[
        viewport& operator =(viewport&&) = default; // ���[�u

        bool initialize(const double width, const double height);

        const  std::shared_ptr<math::matrix44> get_matrix() const { return matrix; }

    private:
        double width;
        double height;

        std::shared_ptr<math::matrix44> matrix;
    };
}
