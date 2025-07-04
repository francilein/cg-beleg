// ==== main.cpp ====
#include "include/camera.hpp"
#include "include/image.hpp"
#include "include/geometry.hpp"
#include "include/obj_loader.hpp"
#include "include/light.hpp"
#include "include/renderer.hpp"
#include "include/kdtree.hpp"
#include <iostream>
#include <chrono>

int main() {
    // Rendering-Einstellungen - Höhere Qualität
    const int width = 1920;
    const int height = 1920;
    
    // Szene laden
    auto scene = load_obj("scenes/twisted_torus_no_numpy.obj", {240, 180, 255});
    
    // Bounding Box der Szene berechnen
    float min_x = 1e30f, max_x = -1e30f;
    float min_y = 1e30f, max_y = -1e30f;
    float min_z = 1e30f, max_z = -1e30f;
    
    for (const auto& tri : scene) {
        min_x = std::min({min_x, tri.v0.x, tri.v1.x, tri.v2.x});
        max_x = std::max({max_x, tri.v0.x, tri.v1.x, tri.v2.x});
        min_y = std::min({min_y, tri.v0.y, tri.v1.y, tri.v2.y});
        max_y = std::max({max_y, tri.v0.y, tri.v1.y, tri.v2.y});
        min_z = std::min({min_z, tri.v0.z, tri.v1.z, tri.v2.z});
        max_z = std::max({max_z, tri.v0.z, tri.v1.z, tri.v2.z});
    }
    
    // Szenen-Zentrum und Größe berechnen
    float center_x = (min_x + max_x) * 0.5f;
    float center_y = (min_y + max_y) * 0.5f;
    float center_z = (min_z + max_z) * 0.5f;
    float size = std::max({max_x - min_x, max_y - min_y, max_z - min_z});
    
    // Kamera nach rechts verschieben um Objekt von rechts anzuschauen
    Point3 cam_pos = {center_x + size * 2.0f, center_y + size * 1.5f, center_z + size * 1.2f};
    Vector3 cam_dir = Vector3({-0.9f, -1.0f, -0.8f}).normalize(); // Nach links und unten blicken
    Camera cam(cam_pos, cam_dir, size * 0.4f, size * 0.4f, width, height); // Kleinerer Sichtbereich = größeres Objekt
    
    // Licht näher positionieren
    Light light = {{center_x, center_y + size * 1.5f, center_z + size * 1.0f}, {255, 255, 255}};
    
    std::cout << "Szenen-Bounding Box:\n";
    std::cout << "  X: [" << min_x << ", " << max_x << "]\n";
    std::cout << "  Y: [" << min_y << ", " << max_y << "]\n";
    std::cout << "  Z: [" << min_z << ", " << max_z << "]\n";
    std::cout << "Kamera positioniert bei (" << cam_pos.x << ", " << cam_pos.y << ", " << cam_pos.z << ") - rechts für Ansicht von rechts\n";
    std::cout << "Licht positioniert bei (" << light.position.x << ", " << light.position.y << ", " << light.position.z << ") - mittig positioniert\n";
    
    // Bild erstellen
    Image img(width, height);
    
    std::cout << "Szene geladen: " << scene.size() << " Dreiecke\n";
    
    // KD-Tree erstellen und aufbauen
    KDTree kdtree;
    auto build_start = std::chrono::high_resolution_clock::now();
    kdtree.build(scene);
    auto build_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> build_time = build_end - build_start;
    std::cout << "KD-Tree Aufbauzeit: " << build_time.count() << " Sekunden\n\n";
    
    // Renderer erstellen und Szene mit KD-Tree rendern
    Renderer renderer(width, height);
    renderer.render_kdtree(kdtree, cam, light, img);
    
    // Bild speichern
    img.save_png("output_torus_view_from_right_hq.png");
    std::cout << "Bild mit KD-Tree gespeichert als output_torus_view_from_right_hq.png ✅\n";
    
    // Optional: Vergleichsrendering ohne KD-Tree
    std::cout << "\nVergleichsrendering ohne KD-Tree...\n";
    Image img_normal(width, height);
    renderer.render(scene, cam, light, img_normal);
    img_normal.save_png("output_torus_view_from_right_hq_normal.png");
    std::cout << "Bild ohne KD-Tree gespeichert als output_torus_view_from_right_hq_normal.png ✅\n";

    return 0;
}
