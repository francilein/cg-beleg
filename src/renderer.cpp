#include "../include/renderer.hpp"
#include "../include/raytracer.hpp"
#include <iostream>
#include <chrono>

void Renderer::show_progress(int current, int total)
{
    float percent = 100.0f * current / (total - 1);
    int bars = static_cast<int>(percent / 2);

    std::cout << "\r[";
    for (int i = 0; i < 50; ++i)
    {
        std::cout << (i < bars ? "█" : " ");
    }
    std::cout << "] " << static_cast<int>(percent) << "%";
    std::cout.flush();
}

void Renderer::render(const std::vector<Triangle> &scene, const Camera &cam,
                      const Light &light, Image &img)
{
    std::cout << "Rendering started...\n";
    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < height; ++y)
    {
        // Fortschrittsanzeige alle paar Zeilen
        if (y % (height / 50) == 0 || y == height - 1)
        {
            show_progress(y, height);
        }

        for (int x = 0; x < width; ++x)
        {
            Ray ray = cam.get_ray(x, y);
            Vector3 color = trace(ray, scene, cam, light);
            img.set_pixel(x, y, Color(static_cast<int>(color.x), static_cast<int>(color.y), static_cast<int>(color.z)));
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> render_time = end - start;

    std::cout << "\nRenderzeit: " << render_time.count() << " Sekunden\n";
}

void Renderer::render_kdtree(const KDTree &kdtree, const Camera &cam,
                             const Light &light, Image &img)
{
    std::cout << "Rendering with KD-Tree started...\n";
    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < height; ++y)
    {
        // Fortschrittsanzeige alle paar Zeilen
        if (y % (height / 50) == 0 || y == height - 1)
        {
            show_progress(y, height);
        }

        for (int x = 0; x < width; ++x)
        {
            Ray ray = cam.get_ray(x, y);
            Vector3 color = trace_kdtree(ray, kdtree, cam, light);
            img.set_pixel(x, y, Color(static_cast<int>(color.x), static_cast<int>(color.y), static_cast<int>(color.z)));
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> render_time = end - start;

    std::cout << "\nKD-Tree Renderzeit: " << render_time.count() << " Sekunden\n";
}
