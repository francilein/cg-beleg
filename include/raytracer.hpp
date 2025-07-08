#pragma once
#include "geometry.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "kdtree.hpp"

// Berechnet die Normale eines Dreiecks
Vector3 compute_normal(const Triangle &tri);

// Phong-Shading für realistische Beleuchtung
Vector3 phong_shading(const Triangle &tri, const Point3 &hitpoint, const Vector3 &normal,
                      const Camera &cam, const Light &light);

// Schatten-Test mit KD-Tree
bool is_in_shadow_kdtree(const Point3 &point, const Light &light, const KDTree &kdtree);

// Hauptfunktion für Raytracing mit KD-Tree
Vector3 trace_kdtree(const Ray &ray, const KDTree &kdtree, const Camera &cam,
                     const Light &light, int depth = 0);

// Hauptfunktion für Raytracing (ohne KD-Tree - für Vergleich)
Vector3 trace(const Ray &ray, const std::vector<Triangle> &scene, const Camera &cam,
              const Light &light, int depth = 0);
