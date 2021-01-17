#pragma once

namespace math {
    class matrix44;

    class vector3 {
    public:
        // コンストラクタ
        vector3();
        vector3(const double x, const double y, const double z) : x(x), y(y), z(z), w(1.0) {}
        vector3(const double x, const double y, const double z, double w) : x(x), y(y), z(z), w(w) {}
        vector3(const vector3&) = default; // コピー
        vector3(vector3&&) = default; // ムーブ

        // デストラクタ
        virtual ~vector3() = default;

        vector3& operator =(const vector3&) = default; // コピー
        vector3& operator =(vector3&&) = default; // ムーブ

        const vector3 operator +(const vector3& rhs) const;
        const vector3 operator -(const vector3& rhs) const;

        const vector3 operator *(const double rhs) const;
        const vector3 operator /(const double rhs) const;

        const vector3 operator *(const matrix44 rhs) const;

        void set(const vector3& vector);
        void set(const double x, const double y, const double z);

        void add(const vector3& vector);
        void add(const double x, const double y, const double z);

        const double get_x() const { return x; }
        const double get_y() const { return y; }
        const double get_z() const { return z; }
        const double get_w() const { return w; }

        const double lenght() const;

        void normalized();
        const vector3 normalize() const;

        const double dot(const vector3& rhs) const;
        const vector3 cross(const vector3& rhs) const;

        const vector3 mult_with_w(const matrix44& rhs) const;

    private:
        double x;
        double y;
        double z;
        double w;
    };

} // math
