// Geometry.h - closest-point-on-triangle with barycentric weights.
//
// Used by the face-projection mapping mode: a source point is projected onto a
// surface triangle and its force is distributed to the three corner nodes by
// the barycentric coordinates of the projection (which sum to 1, so total
// force is conserved exactly).
#pragma once

#include "vm/Vec3.h"

namespace vm {

inline double Dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Closest point on triangle (a,b,c) to p. Returns the point and the barycentric
// weights (wa, wb, wc) of that closest point. Algorithm from Ericson,
// "Real-Time Collision Detection".
inline Vec3 ClosestPointOnTriangle(const Vec3& p, const Vec3& a, const Vec3& b,
                                   const Vec3& c, double& wa, double& wb, double& wc) {
    const Vec3 ab = b - a;
    const Vec3 ac = c - a;
    const Vec3 ap = p - a;

    const double d1 = Dot(ab, ap);
    const double d2 = Dot(ac, ap);
    if (d1 <= 0.0 && d2 <= 0.0) { wa = 1; wb = 0; wc = 0; return a; }

    const Vec3 bp = p - b;
    const double d3 = Dot(ab, bp);
    const double d4 = Dot(ac, bp);
    if (d3 >= 0.0 && d4 <= d3) { wa = 0; wb = 1; wc = 0; return b; }

    const double vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0) {
        const double v = d1 / (d1 - d3);
        wa = 1 - v; wb = v; wc = 0;
        return a + ab * v;
    }

    const Vec3 cp = p - c;
    const double d5 = Dot(ab, cp);
    const double d6 = Dot(ac, cp);
    if (d6 >= 0.0 && d5 <= d6) { wa = 0; wb = 0; wc = 1; return c; }

    const double vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0) {
        const double w = d2 / (d2 - d6);
        wa = 1 - w; wb = 0; wc = w;
        return a + ac * w;
    }

    const double va = d3 * d6 - d5 * d4;
    if (va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0) {
        const double w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        wa = 0; wb = 1 - w; wc = w;
        return b + (c - b) * w;
    }

    // Inside the face.
    const double denom = 1.0 / (va + vb + vc);
    const double v = vb * denom;
    const double w = vc * denom;
    wa = 1 - v - w; wb = v; wc = w;
    return a + ab * v + ac * w;
}

} // namespace vm
