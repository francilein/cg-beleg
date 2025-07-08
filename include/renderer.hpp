#pragma once
#include "camera.hpp"
#include "image.hpp"
#include "geometry.hpp"
#include "light.hpp"
#include "kdtree.hpp"

class Renderer
{
private:
    int width, height;

public:
    Renderer(int w, int h) : width(w), height(h) {}

    // Rendert die Szene mit KD-Tree und zeigt Fortschritt an
    void render_kdtree(const KDTree &kdtree, const Camera &cam,
                       const Light &light, Image &img);

    // Rendert die Szene ohne KD-Tree (für Vergleich)
    void render(const std::vector<Triangle> &scene, const Camera &cam,
                const Light &light, Image &img);

    // Zeigt eine Fortschrittsleiste an
    void show_progress(int current, int total);
};
