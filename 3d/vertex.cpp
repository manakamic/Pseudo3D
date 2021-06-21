#include "color.h"
#include "vector4.h"
#include "vertex.h"

namespace r3d {

    vertex::vertex() {
        position = nullptr;
        uv.fill(0.0);
    }

    bool vertex::initialize() {
        position.reset(new math::vector4);

        return true;
    }

    bool vertex::set_position(const math::vector4& vector) {
        if (position == nullptr) {
            return false;
        }

        position->set(vector);

        return true;
    }
}
