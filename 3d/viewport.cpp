#include "matrix44.h"
#include "viewport.h"

namespace r3d {

    viewport::viewport() {
        width = 0.0;
        height = 0.0;

        matrix = nullptr;
    }

    bool viewport::initialize(const double width, const double height) {
        this->width = width;
        this->height = height;

        matrix.reset(new math::matrix44);

        matrix->viewport(width, height);

        return true;
    }
}
