#pragma once

namespace math {
    class matrix44;

    class vector4 {
    public:
        // �R���X�g���N�^
        vector4();
        vector4(const double x, const double y, const double z) : x(x), y(y), z(z), w(1.0) {}
        vector4(const double x, const double y, const double z, double w) : x(x), y(y), z(z), w(w) {}
        vector4(const vector4&) = default; // �R�s�[
        vector4(vector4&&) = default; // ���[�u

        // �f�X�g���N�^
        virtual ~vector4() = default;

        vector4& operator =(const vector4&) = default; // �R�s�[
        vector4& operator =(vector4&&) = default; // ���[�u

        const vector4 operator +(const vector4& rhs) const;
        const vector4 operator -(const vector4& rhs) const;

        const vector4 operator *(const double rhs) const;
        const vector4 operator /(const double rhs) const;

        const vector4 operator *(const matrix44 rhs) const;

        void set(const vector4& vector);
        void set(const double x, const double y, const double z);

        void add(const vector4& vector);
        void add(const double x, const double y, const double z);

        const double get_x() const { return x; }
        const double get_y() const { return y; }
        const double get_z() const { return z; }
        const double get_w() const { return w; }

        const double lenght() const;

        void normalized();
        const vector4 normalize() const;

        const double dot(const vector4& rhs) const;
        const vector4 cross(const vector4& rhs) const;

        const vector4 mult_with_w(const matrix44& rhs) const;

    private:
        double x;
        double y;
        double z;
        double w;
    };

} // math
