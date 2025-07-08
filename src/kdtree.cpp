#include "../include/kdtree.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

// BoundingBox Implementation
void BoundingBox::expand(const Point3 &point)
{
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);
    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}

void BoundingBox::expand(const BoundingBox &box)
{
    expand(box.min);
    expand(box.max);
}

bool BoundingBox::intersect(const Ray &ray, float &t_min, float &t_max) const
{
    t_min = 0.0f;
    t_max = 1e30f;

    // Teste alle drei Achsen
    for (int i = 0; i < 3; i++)
    {
        float ray_origin = (i == 0) ? ray.origin.x : (i == 1) ? ray.origin.y
                                                              : ray.origin.z;
        float ray_dir = (i == 0) ? ray.direction.x : (i == 1) ? ray.direction.y
                                                              : ray.direction.z;
        float box_min = (i == 0) ? min.x : (i == 1) ? min.y
                                                    : min.z;
        float box_max = (i == 0) ? max.x : (i == 1) ? max.y
                                                    : max.z;

        if (std::abs(ray_dir) < 1e-8f)
        {
            // Strahl ist parallel zur Achse
            if (ray_origin < box_min || ray_origin > box_max)
            {
                return false;
            }
        }
        else
        {
            // Berechne Schnittpunkte
            float t1 = (box_min - ray_origin) / ray_dir;
            float t2 = (box_max - ray_origin) / ray_dir;

            if (t1 > t2)
                std::swap(t1, t2);

            t_min = std::max(t_min, t1);
            t_max = std::min(t_max, t2);

            if (t_min > t_max)
                return false;
        }
    }

    return t_max > 0.001f; // Mindest-Distanz
}

float BoundingBox::surface_area() const
{
    Vector3 d = max - min;
    return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
}

int BoundingBox::longest_axis() const
{
    Vector3 d = max - min;
    if (d.x > d.y && d.x > d.z)
        return 0;
    if (d.y > d.z)
        return 1;
    return 2;
}

// KDTree Implementation
KDTree::KDTree(int max_depth, int max_triangles_per_leaf)
    : max_depth(std::min(max_depth, 15)), max_triangles_per_leaf(std::max(max_triangles_per_leaf, 20))
{
    // Begrenze die Parameter für große Modelle
}

void KDTree::build(const std::vector<Triangle> &triangles)
{
    std::cout << "Building KD-Tree with " << triangles.size() << " triangles...\n";

    root = std::make_unique<KDNode>();

    // Zeiger auf alle Dreiecke erstellen
    std::vector<const Triangle *> triangle_ptrs;
    triangle_ptrs.reserve(triangles.size());
    for (const auto &tri : triangles)
    {
        triangle_ptrs.push_back(&tri);
    }

    // Bounding Box der gesamten Szene berechnen
    root->bbox = compute_bbox(triangle_ptrs);

    // Rekursiv aufbauen
    build_recursive(root.get(), triangle_ptrs, 0);

    std::cout << "KD-Tree built successfully!\n";
    print_stats();
}

void KDTree::build_recursive(KDNode *node, std::vector<const Triangle *> &triangles, int depth)
{
    // Debugging-Ausgabe
    if (depth == 0)
    {
        std::cout << "Starte KD-Tree Aufbau mit " << triangles.size() << " Dreiecken\n";
        if (triangles.size() > 100000)
        {
            std::cout << "Warnung: Sehr große Szene! Verwende konservative Einstellungen.\n";
        }
    }

    // Stopp-Kriterien - konservativer für große Szenen
    int effective_max_triangles = triangles.size() > 100000 ? 100 : max_triangles_per_leaf;
    int effective_max_depth = triangles.size() > 100000 ? 10 : max_depth;

    if (depth >= effective_max_depth || triangles.size() <= effective_max_triangles)
    {
        node->is_leaf = true;
        node->triangles = triangles;
        return;
    }

    // Null-Pointer Check
    if (triangles.empty())
    {
        node->is_leaf = true;
        return;
    }

    // Beste Teilungsachse finden
    int best_axis = node->bbox.longest_axis();
    float best_pos = 0.0f;
    float best_cost = 1e30f;

    // Verschiedene Teilungspositionen testen
    for (int axis = 0; axis < 3; axis++)
    {
        float min_coord = (axis == 0) ? node->bbox.min.x : (axis == 1) ? node->bbox.min.y
                                                                       : node->bbox.min.z;
        float max_coord = (axis == 0) ? node->bbox.max.x : (axis == 1) ? node->bbox.max.y
                                                                       : node->bbox.max.z;

        // Division durch Null vermeiden
        if (std::abs(max_coord - min_coord) < 1e-8f)
        {
            continue;
        }

        for (int i = 1; i < 4; i++)
        {
            float pos = min_coord + (max_coord - min_coord) * i / 4.0f;
            float cost = evaluate_split(triangles, axis, pos);

            if (cost < best_cost)
            {
                best_cost = cost;
                best_axis = axis;
                best_pos = pos;
            }
        }
    }

    // Dreiecke aufteilen - Dreiecke können in beiden Hälften sein!
    std::vector<const Triangle *> left_triangles, right_triangles;

    for (const Triangle *tri : triangles)
    {
        if (!tri)
            continue; // Null-Pointer Check

        BoundingBox tri_bbox = compute_triangle_bbox(*tri);

        // Prüfe, ob das Dreieck die linke Hälfte überlappt
        bool overlaps_left = (best_axis == 0) ? (tri_bbox.min.x < best_pos) : (best_axis == 1) ? (tri_bbox.min.y < best_pos)
                                                                                               : (tri_bbox.min.z < best_pos);

        // Prüfe, ob das Dreieck die rechte Hälfte überlappt
        bool overlaps_right = (best_axis == 0) ? (tri_bbox.max.x > best_pos) : (best_axis == 1) ? (tri_bbox.max.y > best_pos)
                                                                                                : (tri_bbox.max.z > best_pos);

        // Dreieck in beide Hälften einfügen, wenn es beide überlappt
        if (overlaps_left)
        {
            left_triangles.push_back(tri);
        }
        if (overlaps_right)
        {
            right_triangles.push_back(tri);
        }
    }

    // Falls schlechte Teilung, als Blatt behandeln
    if (left_triangles.empty() || right_triangles.empty())
    {
        node->is_leaf = true;
        node->triangles = triangles;
        return;
    }

    // Memory-Check
    if (left_triangles.size() > 50000 || right_triangles.size() > 50000)
    {
        std::cout << "Warnung: Sehr große Teilung bei Tiefe " << depth << "\n";
        std::cout << "Links: " << left_triangles.size() << ", Rechts: " << right_triangles.size() << "\n";
        node->is_leaf = true;
        node->triangles = triangles;
        return;
    }

    // Kindknoten erstellen
    node->is_leaf = false;
    node->axis = best_axis;
    node->split_pos = best_pos;

    try
    {
        node->left = std::make_unique<KDNode>();
        node->right = std::make_unique<KDNode>();
    }
    catch (const std::bad_alloc &e)
    {
        std::cout << "Memory-Fehler beim Erstellen der Kindknoten\n";
        node->is_leaf = true;
        node->triangles = triangles;
        return;
    }

    // Bounding Boxes für Kindknoten berechnen
    node->left->bbox = node->bbox;
    node->right->bbox = node->bbox;

    if (best_axis == 0)
    {
        node->left->bbox.max.x = best_pos;
        node->right->bbox.min.x = best_pos;
    }
    else if (best_axis == 1)
    {
        node->left->bbox.max.y = best_pos;
        node->right->bbox.min.y = best_pos;
    }
    else
    {
        node->left->bbox.max.z = best_pos;
        node->right->bbox.min.z = best_pos;
    }

    // Rekursiv weiterbauen
    build_recursive(node->left.get(), left_triangles, depth + 1);
    build_recursive(node->right.get(), right_triangles, depth + 1);
}

BoundingBox KDTree::compute_bbox(const std::vector<const Triangle *> &triangles) const
{
    BoundingBox bbox;
    for (const Triangle *tri : triangles)
    {
        bbox.expand(tri->v0);
        bbox.expand(tri->v1);
        bbox.expand(tri->v2);
    }
    return bbox;
}

BoundingBox KDTree::compute_triangle_bbox(const Triangle &tri) const
{
    BoundingBox bbox;

    // Prüfe auf gültige Werte
    if (std::isnan(tri.v0.x) || std::isnan(tri.v0.y) || std::isnan(tri.v0.z) ||
        std::isnan(tri.v1.x) || std::isnan(tri.v1.y) || std::isnan(tri.v1.z) ||
        std::isnan(tri.v2.x) || std::isnan(tri.v2.y) || std::isnan(tri.v2.z))
    {
        std::cout << "Warnung: NaN-Werte im Dreieck gefunden!\n";
        return bbox;
    }

    bbox.expand(tri.v0);
    bbox.expand(tri.v1);
    bbox.expand(tri.v2);
    return bbox;
}

float KDTree::evaluate_split(const std::vector<const Triangle *> &triangles, int axis, float pos) const
{
    // Null-Check
    if (triangles.empty())
        return 1e30f;

    int left_count = 0, right_count = 0;
    int overlap_count = 0;

    for (const Triangle *tri : triangles)
    {
        if (!tri)
            continue; // Null-Pointer Check

        BoundingBox tri_bbox = compute_triangle_bbox(*tri);

        // Prüfe Überlappung mit beiden Seiten
        bool overlaps_left = (axis == 0) ? (tri_bbox.min.x < pos) : (axis == 1) ? (tri_bbox.min.y < pos)
                                                                                : (tri_bbox.min.z < pos);

        bool overlaps_right = (axis == 0) ? (tri_bbox.max.x > pos) : (axis == 1) ? (tri_bbox.max.y > pos)
                                                                                 : (tri_bbox.max.z > pos);

        if (overlaps_left)
            left_count++;
        if (overlaps_right)
            right_count++;
        if (overlaps_left && overlaps_right)
            overlap_count++;
    }

    // Vermeide schlechte Teilungen
    if (left_count == 0 || right_count == 0)
        return 1e30f;

    // Kosten basierend auf Anzahl der Dreiecke + Überlappungsstrafe
    return left_count + right_count + overlap_count * 2.0f;
}

bool KDTree::intersect(const Ray &ray, float &t, const Triangle *&hit_triangle) const
{
    if (!root)
        return false;

    float min_t = 1e30f;
    const Triangle *closest_triangle = nullptr;

    if (intersect_recursive(root.get(), ray, min_t, closest_triangle))
    {
        t = min_t;
        hit_triangle = closest_triangle;
        return true;
    }

    return false;
}

bool KDTree::intersect_recursive(const KDNode *node, const Ray &ray, float &min_t, const Triangle *&hit_triangle) const
{
    // Bounding Box Test
    float t_min, t_max;
    if (!node->bbox.intersect(ray, t_min, t_max) || t_min > min_t)
    {
        return false;
    }

    bool hit = false;

    if (node->is_leaf)
    {
        // Blatt: Alle Dreiecke testen
        for (const Triangle *tri : node->triangles)
        {
            float t;
            if (tri->intersect(ray, t) && t < min_t && t > 0.001f)
            {
                min_t = t;
                hit_triangle = tri;
                hit = true;
            }
        }
    }
    else
    {
        // Innerer Knoten: Beide Kinder testen
        // Vereinfachte Traversierung - teste beide Kinder
        if (node->left && intersect_recursive(node->left.get(), ray, min_t, hit_triangle))
        {
            hit = true;
        }
        if (node->right && intersect_recursive(node->right.get(), ray, min_t, hit_triangle))
        {
            hit = true;
        }
    }

    return hit;
}

void KDTree::print_stats() const
{
    if (!root)
        return;

    int leaf_count = 0;
    int total_triangles = 0;
    int max_depth = 0;
    print_stats_recursive(root.get(), 0, leaf_count, total_triangles, max_depth);

    std::cout << "KD-Tree Statistics:\n";
    std::cout << "  Leaf nodes: " << leaf_count << "\n";
    std::cout << "  Total triangles in leaves: " << total_triangles << "\n";
    std::cout << "  Average triangles per leaf: " << (float)total_triangles / leaf_count << "\n";
    std::cout << "  Maximum depth: " << max_depth << "\n";
}

void KDTree::print_stats_recursive(const KDNode *node, int depth, int &leaf_count, int &total_triangles, int &max_depth) const
{
    max_depth = std::max(max_depth, depth);

    if (node->is_leaf)
    {
        leaf_count++;
        total_triangles += node->triangles.size();
    }
    else
    {
        if (node->left)
            print_stats_recursive(node->left.get(), depth + 1, leaf_count, total_triangles, max_depth);
        if (node->right)
            print_stats_recursive(node->right.get(), depth + 1, leaf_count, total_triangles, max_depth);
    }
}