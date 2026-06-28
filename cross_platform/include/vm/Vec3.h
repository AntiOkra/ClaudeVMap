// Vec3.h - portable 3D vector (MFC-free replacement for the CMzPoint subset
// actually used by VectorMapping: set / add / sub / scale / distance / cross).
#pragma once

#include <cmath>

namespace vm {

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vec3() = default;
    Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    void Set(double x_, double y_, double z_) { x = x_; y = y_; z = z_; }

    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(double s)      const { return {x * s, y * s, z * s}; }

    double LengthSquared() const { return x * x + y * y + z * z; }
    double Length()        const { return std::sqrt(LengthSquared()); }

    double DistanceSquared(const Vec3& o) const {
        const double dx = x - o.x, dy = y - o.y, dz = z - o.z;
        return dx * dx + dy * dy + dz * dz;
    }
    double Distance(const Vec3& o) const { return std::sqrt(DistanceSquared(o)); }

    Vec3 CrossProduct(const Vec3& o) const {
        return {
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        };
    }
};

// Area of a triangle (matches GeneralFunction::AreaTriangle: |(p2-p1) x (p0-p1)| / 2).
inline double AreaTriangle(const Vec3& p0, const Vec3& p1, const Vec3& p2) {
    const Vec3 v0 = p2 - p1;
    const Vec3 v1 = p0 - p1;
    return v0.CrossProduct(v1).Length() / 2.0;
}

// Area of a quad as two triangles (matches GeneralFunction::AreaQuad).
inline double AreaQuad(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3) {
    return AreaTriangle(p0, p1, p2) + AreaTriangle(p2, p3, p0);
}

} // namespace vm
