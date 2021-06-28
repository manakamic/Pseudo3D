#include "color.h"
#include "vector4.h"
#include "light.h"

namespace r3d {

    light::light() {
        direction = nullptr;
    }

    bool light::initialize() {
        direction.reset(new math::vector4);

        return true;
    }

    bool light::set_direction(const math::vector4& vector) {
        if (direction == nullptr) {
            return false;
        }

        // —\‚ß”½“]‚µ‚Ä‚¨‚­
        direction->set(-vector.get_x(), -vector.get_y(), -vector.get_z());

        return true;
    }
}
