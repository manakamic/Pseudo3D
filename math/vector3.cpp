#include <cmath>
#include "vector3.h"
#include "matrix44.h"

namespace math {

    vector3::vector3() {
        x = 0.0l; y = 0.0; z = 0.0; w = 1.0;
    }

    void vector3::set(const vector3& vector) {
        x = vector.get_x();
        y = vector.get_y();
        z = vector.get_z();
        w = vector.get_w();
    }

    void vector3::set(const double x, const double y, const double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void vector3::add(const vector3& vector) {
        x += vector.get_x();
        y += vector.get_y();
        z += vector.get_z();
    }

    void vector3::add(const double x, const double y, const double z) {
        this->x += x;
        this->y += y;
        this->z += z;
    }

    const double vector3::lenght() const {
        return sqrt(x * x + y * y + z * z);
    }

    void vector3::normalized() {
        auto len = lenght();

        x /= len;
        y /= len;
        z /= len;
    }

    const vector3 vector3::normalize() const {
        auto len = lenght();
        auto nx = x / len;
        auto ny = y / len;
        auto nz = z / len;

        return vector3(nx, ny, nz);
    }

    const double vector3::dot(const vector3& rhs) const {
        return x * rhs.get_x() + y * rhs.get_y() + z * rhs.get_z();
    }

    const vector3 vector3::cross(const vector3& rhs) const {
        auto cx = y * rhs.get_z() - z * rhs.get_y();
        auto cy = z * rhs.get_x() - x * rhs.get_z();
        auto cz = x * rhs.get_y() - y * rhs.get_x();

        return vector3(cx, cy, cz);
    }

    const vector3 vector3::operator +(const vector3& rhs) const {
        return vector3(x + rhs.get_x(), y + rhs.get_y(), z + rhs.get_z());
    }

    const vector3 vector3::operator -(const vector3& rhs) const {
        return vector3(x - rhs.get_x(), y - rhs.get_y(), z - rhs.get_z());
    }

    const vector3 vector3::operator *(const double rhs) const {
        return vector3(x * rhs, y * rhs, z * rhs);
    }

    const vector3 vector3::operator /(const double rhs) const {
        return vector3(x / rhs, y / rhs, z / rhs);
    }

    const vector3 vector3::operator *(const matrix44 rhs) const {
        auto mx = x * rhs.get_value(0, 0)
                + y * rhs.get_value(1, 0)
                + z * rhs.get_value(2, 0)
                +     rhs.get_value(3, 0);

        auto my = x * rhs.get_value(0, 1)
                + y * rhs.get_value(1, 1)
                + z * rhs.get_value(2, 1)
                +     rhs.get_value(3, 1);

        auto mz = x * rhs.get_value(0, 2)
                + y * rhs.get_value(1, 2)
                + z * rhs.get_value(2, 2)
                +     rhs.get_value(3, 2);

        return vector3(mx, my, mz);
    }

    const vector3 vector3::mult_with_w(const matrix44& rhs) const {
        auto xyz = *this * rhs;
        auto w = x * rhs.get_value(0, 3)
               + y * rhs.get_value(1, 3)
               + z * rhs.get_value(2, 3)
               +     rhs.get_value(3, 3);

        auto xyzw = xyz / w;

        return vector3(xyzw.x, xyzw.y, xyzw.z, w);
    }

} // math
