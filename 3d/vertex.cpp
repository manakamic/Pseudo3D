#include "color.h"
#include "vector3.h"
#include "vertex.h"

namespace r3d {

    vertex::vertex() {
        position = nullptr;
        uv.fill(0.0);
    }

    bool vertex::initialize() {
        position.reset(new math::vector3);

        return true;
    }

    bool vertex::set_position(const math::vector3& vector) {
        if (position == nullptr) {
            return false;
        }

        position->set(vector);

        return true;
    }
}
