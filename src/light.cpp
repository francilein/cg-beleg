#include "../include/light.hpp"
#include "../include/geometry.hpp"

bool is_in_shadow(const Point3 &point, const Light &light, const std::vector<Triangle> &scene)
{
    Vector3 dir = (light.position - point).normalize();
    Ray shadow_ray(point + dir * 0.001f, dir);
    float dist_to_light = (light.position - point).length();

    for (const auto &tri : scene)
    {
        float t;
        if (tri.intersect(shadow_ray, t) && t < dist_to_light)
        {
            return true;
        }
    }
    return false;
}