// ==== main.cpp ====
#include "include/camera.hpp"
#include "include/image.hpp"
#include "include/geometry.hpp"
#include "include/obj_loader.hpp"
#include <limits>
#include <cmath>
#include <chrono>
#include <iostream>


struct Light {
    Point3 position;
    Vector3 color;
};

bool is_in_shadow(const Point3& point, const Light& light, const std::vector<Triangle>& scene) {
    Vector3 dir = (light.position - point).normalize();
    Ray shadow_ray(point + dir * 0.001f, dir);
    float dist_to_light = (light.position - point).length();
    for (const auto& tri : scene) {
        float t;
        if (tri.intersect(shadow_ray, t) && t < dist_to_light) return true;
    }
    return false;
}

Vector3 compute_normal(const Triangle& tri) {
    Vector3 edge1 = tri.v1 - tri.v0;
    Vector3 edge2 = tri.v2 - tri.v0;
    return edge1.cross(edge2).normalize();
}

Vector3 phong_shading(const Triangle& tri, const Point3& hitpoint, const Vector3& normal, const Camera& cam, const Light& light) {
    Vector3 ambient = tri.color * 0.3f;
    Vector3 to_light = (light.position - hitpoint).normalize();
    Vector3 to_view = (cam.eye - hitpoint).normalize();
    Vector3 reflect_dir = (normal * 2.0f * normal.dot(to_light) - to_light).normalize();
    float diff = std::max(0.0f, normal.dot(to_light));
    float spec = std::pow(std::max(0.0f, reflect_dir.dot(to_view)), 32.0f);
    Vector3 diffuse = tri.color * diff * 0.8f;
    Vector3 specular = light.color * spec * 0.5f;
    return ambient + diffuse + specular;
}

Vector3 trace(const Ray& ray, const std::vector<Triangle>& scene, const Camera& cam, const Light& light, int depth = 0) {
    if (depth > 3) return {30, 60, 100};

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

    if (!hit_tri) return {30, 60, 100};
    Vector3 normal = compute_normal(*hit_tri);
    Vector3 color;
    if (is_in_shadow(hit_point, light, scene)) {
        color = hit_tri->color * 0.2f;
    } else {
        color = phong_shading(*hit_tri, hit_point, normal, cam, light);
    }

    Vector3 reflect_dir = ray.direction - normal * 2.0f * ray.direction.dot(normal);
    Ray reflected_ray(hit_point + reflect_dir * 0.001f, reflect_dir);
    Vector3 reflection = trace(reflected_ray, scene, cam, light, depth + 1);

    float reflectivity = 0.3f;
    return color * (1.0f - reflectivity) + reflection * reflectivity;
}

int main() {
    int w = 512, h = 512;
    Camera cam({0, 0, -5}, {0, 0, 1}, 2.0f, 2.0f, w, h);
    Image img(w, h);

    auto scene = load_obj("scenes/car_model.obj", {180, 180, 255});
    Light light = {{0, 2, -2}, {255, 255, 255}};

    std::cout << "Rendering started...\n";
    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < h; ++y) {
        // Fortschrittsanzeige alle paar Zeilen
        if (y % (h / 50) == 0 || y == h - 1) {
            float percent = 100.0f * y / (h - 1);
            int bars = static_cast<int>(percent / 2);
            std::cout << "\r[";
            for (int i = 0; i < 50; ++i)
                std::cout << (i < bars ? "█" : " ");
            std::cout << "] " << static_cast<int>(percent) << "%";
            std::cout.flush();
        }

        for (int x = 0; x < w; ++x) {
            Ray ray = cam.get_ray(x, y);
            Vector3 color = trace(ray, scene, cam, light);
            img.set_pixel(x, y, Color(static_cast<int>(color.x), static_cast<int>(color.y), static_cast<int>(color.z)));
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> render_time = end - start;

    std::cout << "\nRenderzeit: " << render_time.count() << " Sekunden\n";

    img.save_png("output.png");
    std::cout << "Bild gespeichert als output.png ✅\n";

    return 0;
}
