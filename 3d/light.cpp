#include "color.h"
#include "vector4.h"
#include "light.h"

namespace r3d {

    light::light() {
        direction = nullptr;
        ambient = nullptr;
        diffuse = nullptr;
        speculer = nullptr;
    }

    bool light::initialize() {
        direction.reset(new math::vector4);
        ambient.reset(new image::color);
        diffuse.reset(new image::color);
        speculer.reset(new image::color);

        return true;
    }

    bool light::set_direction(const math::vector4& vector) {
        if (direction == nullptr) {
            return false;
        }

        direction->set(vector);

        return true;
    }

    bool light::set_ambient(const image::color& color) {
        if (ambient == nullptr) {
            return false;
        }

        ambient->set(color);

        return true;
    }

    bool light::set_diffuse(const image::color& color) {
        if (diffuse == nullptr) {
            return false;
        }

        diffuse->set(color);

        return true;
    }

    bool light::set_speculer(const image::color& color) {
        if (speculer == nullptr) {
            return false;
        }

        speculer->set(color);

        return true;
    }
}
