#include "include/geometry.hpp"
#include "include/camera.hpp"
#include "include/image.hpp"

int main() {
    int w = 512, h = 512;
    Camera cam({0,0,-5}, {0,0,1}, 2.0f, 2.0f, w, h);
    Image img(w, h);

    std::vector<Triangle> scene = {
        Triangle({-1,-1,2}, {1,-1,2}, {0,1,2}, {255, 0, 0})
    };

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Ray ray = cam.get_ray(x, y);
            float min_t = std::numeric_limits<float>::max();
            Vector3 color(0,0,0);
            for (const auto& tri : scene) {
                float t;
                if (tri.intersect(ray, t) && t < min_t) {
                    min_t = t;
                    color = tri.color;
                }
            }
            img.set_pixel(x, y, Color((int)color.x, (int)color.y, (int)color.z));
        }
    }

    img.save_ppm("output.ppm");
    return 0;
}
