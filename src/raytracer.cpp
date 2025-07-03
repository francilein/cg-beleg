#include "../include/raytracer.hpp"
#include <limits>
#include <cmath>
#include <algorithm>

Vector3 compute_normal(const Triangle& tri) {
    Vector3 edge1 = tri.v1 - tri.v0;
    Vector3 edge2 = tri.v2 - tri.v0;
    return edge1.cross(edge2).normalize();
}

Vector3 phong_shading(const Triangle& tri, const Point3& hitpoint, const Vector3& normal, 
                     const Camera& cam, const Light& light) {
    // Ambiente Beleuchtung
    Vector3 ambient = tri.color * 0.3f;
    
    // Diffuse Beleuchtung
    Vector3 to_light = (light.position - hitpoint).normalize();
    float diff = std::max(0.0f, normal.dot(to_light));
    Vector3 diffuse = tri.color * diff * 0.8f;
    
    // Spekulare Beleuchtung
    Vector3 to_view = (cam.eye - hitpoint).normalize();
    Vector3 reflect_dir = (normal * 2.0f * normal.dot(to_light) - to_light).normalize();
    float spec = std::pow(std::max(0.0f, reflect_dir.dot(to_view)), 32.0f);
    Vector3 specular = light.color * spec * 0.5f;
    
    return ambient + diffuse + specular;
}

Vector3 trace(const Ray& ray, const std::vector<Triangle>& scene, const Camera& cam, 
             const Light& light, int depth) {
    // Rekursionslimit für Reflexionen
    if (depth > 3) {
        return {30, 60, 100}; // Hintergrundfarbe
    }

    // Nächste Schnittstelle finden
    float min_t = std::numeric_limits<float>::max();
    const Triangle* hit_tri = nullptr;
    Point3 hit_point;

    for (const auto& tri : scene) {
        float t;
        if (tri.intersect(ray, t) && t < min_t) {
            min_t = t;
            hit_tri = &tri;
            hit_point = ray.origin + ray.direction * t;
        }
    }

    // Kein Treffer - Hintergrundfarbe zurückgeben
    if (!hit_tri) {
        return {30, 60, 100};
    }

    // Normale berechnen und Beleuchtung
    Vector3 normal = compute_normal(*hit_tri);
    Vector3 color;
    
    if (is_in_shadow(hit_point, light, scene)) {
        // Schatten - nur ambiente Beleuchtung
        color = hit_tri->color * 0.2f;
    } else {
        // Vollständige Phong-Beleuchtung
        color = phong_shading(*hit_tri, hit_point, normal, cam, light);
    }

    // Reflexion berechnen
    Vector3 reflect_dir = ray.direction - normal * 2.0f * ray.direction.dot(normal);
    Ray reflected_ray(hit_point + reflect_dir * 0.001f, reflect_dir);
    Vector3 reflection = trace(reflected_ray, scene, cam, light, depth + 1);

    // Reflexion mit Grundfarbe mischen
    float reflectivity = 0.3f;
    return color * (1.0f - reflectivity) + reflection * reflectivity;
}

bool is_in_shadow_kdtree(const Point3& point, const Light& light, const KDTree& kdtree) {
    Vector3 dir = (light.position - point).normalize();
    Ray shadow_ray(point + dir * 0.001f, dir);
    float dist_to_light = (light.position - point).length();
    
    float t;
    const Triangle* hit_triangle;
    if (kdtree.intersect(shadow_ray, t, hit_triangle) && t < dist_to_light) {
        return true;
    }
    return false;
}

Vector3 trace_kdtree(const Ray& ray, const KDTree& kdtree, const Camera& cam, 
                    const Light& light, int depth) {
    // Rekursionslimit für Reflexionen
    if (depth > 3) {
        return {30, 60, 100}; // Hintergrundfarbe
    }

    // Nächste Schnittstelle mit KD-Tree finden
    float min_t;
    const Triangle* hit_tri = nullptr;
    
    if (!kdtree.intersect(ray, min_t, hit_tri)) {
        return {30, 60, 100}; // Hintergrundfarbe
    }

    Point3 hit_point = ray.origin + ray.direction * min_t;
    Vector3 normal = compute_normal(*hit_tri);
    Vector3 color;
    
    if (is_in_shadow_kdtree(hit_point, light, kdtree)) {
        // Schatten - nur ambiente Beleuchtung
        color = hit_tri->color * 0.2f;
    } else {
        // Vollständige Phong-Beleuchtung
        color = phong_shading(*hit_tri, hit_point, normal, cam, light);
    }

    // Reflexion berechnen
    Vector3 reflect_dir = ray.direction - normal * 2.0f * ray.direction.dot(normal);
    Ray reflected_ray(hit_point + reflect_dir * 0.001f, reflect_dir);
    Vector3 reflection = trace_kdtree(reflected_ray, kdtree, cam, light, depth + 1);

    // Reflexion mit Grundfarbe mischen
    float reflectivity = 0.3f;
    return color * (1.0f - reflectivity) + reflection * reflectivity;
}
