#include "color.h"
#include "vector4.h"
#include "vertex.h"
#if defined(_USE_LIGHTING)
#include "color.h"
#endif

namespace r3d {

    vertex::vertex() {
        position = nullptr;

        uv.fill(0.0);

#if defined(_USE_LIGHTING)
        world_position = nullptr;
        normal = nullptr;
#if defined(_USE_NORMAL_MAP)
        tangent = nullptr;
        binormal = nullptr;
#endif
        diffuse = nullptr;
        speculer = nullptr;
        speculer_power = 1.0;
#endif
    }

    bool vertex::initialize() {
        position.reset(new math::vector4);
#if defined(_USE_LIGHTING)
        world_position.reset(new math::vector4);
        normal.reset(new math::vector4);
#if defined(_USE_NORMAL_MAP)
        tangent.reset(new math::vector4);
        binormal.reset(new math::vector4);
#endif
        diffuse.reset(new image::color);
        speculer.reset(new image::color);
#endif

        return true;
    }

    bool vertex::set_position(const math::vector4& vector) {
        if (position == nullptr) {
            return false;
        }

        position->set(vector);

        return true;
    }

#if defined(_USE_LIGHTING)
    bool vertex::set_world_position(const math::vector4& vector) {
        if (world_position == nullptr) {
            return false;
        }

        world_position->set(vector);

        return true;
    }

    bool vertex::set_normal(const math::vector4& vector) {
        if (normal == nullptr) {
            return false;
        }

        normal->set(vector);

        return true;
    }

#if defined(_USE_NORMAL_MAP)
    bool vertex::set_tangent(const math::vector4& vector) {
        if (tangent == nullptr) {
            return false;
        }

        tangent->set(vector);

        return true;
    }

    bool vertex::set_binormal(const math::vector4& vector) {
        if (binormal == nullptr) {
            return false;
        }

        binormal->set(vector);

        return true;
    }
#endif

    bool vertex::set_diffuse(const image::color& color) {
        if (diffuse == nullptr) {
            return false;
        }

        diffuse->set(color);

        return true;
    }

    bool vertex::set_speculer(const image::color& color, double power) {
        if (speculer == nullptr) {
            return false;
        }

        speculer->set(color);
        speculer_power = power;

        return true;
    }
#endif
}
