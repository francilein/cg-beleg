#pragma once
#include <vector>
#include <cmath>
#include <limits>

struct Vector3 {
    float x, y, z;
    Vector3(float x=0, float y=0, float z=0): x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& v) const { return Vector3(x+v.x, y+v.y, z+v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x-v.x, y-v.y, z-v.z); }
    Vector3 operator*(float s) const { return Vector3(x*s, y*s, z*s); }
    Vector3 operator/(float s) const { return Vector3(x/s, y/s, z/s); }

    float dot(const Vector3& v) const { return x*v.x + y*v.y + z*v.z; }
    Vector3 cross(const Vector3& v) const {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    float length() const { return std::sqrt(x*x + y*y + z*z); }
    Vector3 normalize() const { float len = length(); return (len > 0) ? *this / len : *this; }
};

using Point3 = Vector3;

struct Ray {
    Point3 origin;
    Vector3 direction;
    Ray(Point3 origin, Vector3 direction) : origin(origin), direction(direction.normalize()) {}
};

struct Triangle {
    Point3 v0, v1, v2;
    Vector3 color;

    Triangle(Point3 a, Point3 b, Point3 c, Vector3 color) : v0(a), v1(b), v2(c), color(color) {}

    bool intersect(const Ray& ray, float& t) const {
        const float EPS = 1e-6;
        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;
        Vector3 h = ray.direction.cross(edge2);
        float a = edge1.dot(h);
        if (std::abs(a) < EPS) return false;

        float f = 1.0 / a;
        Vector3 s = ray.origin - v0;
        float u = f * s.dot(h);
        if (u < 0.0 || u > 1.0) return false;

        Vector3 q = s.cross(edge1);
        float v = f * ray.direction.dot(q);
        if (v < 0.0 || u + v > 1.0) return false;

        t = f * edge2.dot(q);
        return t > EPS;
    }
};
