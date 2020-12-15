#include "matrix44.h"
#include "perspective.h"

namespace r3d {

    perspective::perspective() {
        fov_y = 0.0;
        aspect = 0.0;
        near_z = 0.0;
        far_z = 0.0;

        matrix = nullptr;
    }

    bool perspective::initialize(const double fov_y, const double aspect, const double near_z, const double far_z) {
        if (near_z >= far_z) {
            return false;
        }

        this->fov_y   = fov_y;
        this->aspect = aspect;
        this->near_z  = near_z;
        this->far_z   = far_z;

        matrix.reset(new math::matrix44);

        matrix->perspective(fov_y, aspect, near_z, far_z);

        return true;
    }
}
