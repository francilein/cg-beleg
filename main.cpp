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
    // Rendering-Einstellungen
    const int width = 512;
    const int height = 512;
    
    // Kamera erstellen
    Camera cam({0, 0, -5}, {0, 0, 1}, 2.0f, 2.0f, width, height);
    
    // Bild erstellen
    Image img(width, height);
    
    // Szene und Licht laden
    auto scene = load_obj("scenes/twisted_torus_no_numpy.obj", {180, 180, 255});
    Light light = {{0, 5, -10}, {255, 255, 255}};
    
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
    img.save_png("output_kdtree.png");
    std::cout << "Bild mit KD-Tree gespeichert als output_kdtree.png ✅\n";
    
    // Optional: Vergleichsrendering ohne KD-Tree
    std::cout << "\nVergleichsrendering ohne KD-Tree...\n";
    Image img_normal(width, height);
    renderer.render(scene, cam, light, img_normal);
    img_normal.save_png("output_normal.png");
    std::cout << "Bild ohne KD-Tree gespeichert als output_normal.png ✅\n";

    return 0;
}
