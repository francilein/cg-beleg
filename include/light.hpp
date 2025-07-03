#pragma once
#include "geometry.hpp"

struct Light {
    Point3 position;
    Vector3 color;
};

// Prüft, ob ein Punkt im Schatten liegt
bool is_in_shadow(const Point3& point, const Light& light, const std::vector<Triangle>& scene);
