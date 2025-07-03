#include "../include/kdtree.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

// BoundingBox Implementation
void BoundingBox::expand(const Point3& point) {
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);
    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}

void BoundingBox::expand(const BoundingBox& box) {
    expand(box.min);
    expand(box.max);
}

bool BoundingBox::intersect(const Ray& ray, float& t_min, float& t_max) const {
    float t1, t2;
    t_min = -1e30f;
    t_max = 1e30f;
    
    // X-Achse
    if (std::abs(ray.direction.x) > 1e-8f) {
        t1 = (min.x - ray.origin.x) / ray.direction.x;
        t2 = (max.x - ray.origin.x) / ray.direction.x;
        if (t1 > t2) std::swap(t1, t2);
        t_min = std::max(t_min, t1);
        t_max = std::min(t_max, t2);
        if (t_min > t_max) return false;
    } else if (ray.origin.x < min.x || ray.origin.x > max.x) {
        return false;
    }
    
    // Y-Achse
    if (std::abs(ray.direction.y) > 1e-8f) {
        t1 = (min.y - ray.origin.y) / ray.direction.y;
        t2 = (max.y - ray.origin.y) / ray.direction.y;
        if (t1 > t2) std::swap(t1, t2);
        t_min = std::max(t_min, t1);
        t_max = std::min(t_max, t2);
        if (t_min > t_max) return false;
    } else if (ray.origin.y < min.y || ray.origin.y > max.y) {
        return false;
    }
    
    // Z-Achse
    if (std::abs(ray.direction.z) > 1e-8f) {
        t1 = (min.z - ray.origin.z) / ray.direction.z;
        t2 = (max.z - ray.origin.z) / ray.direction.z;
        if (t1 > t2) std::swap(t1, t2);
        t_min = std::max(t_min, t1);
        t_max = std::min(t_max, t2);
        if (t_min > t_max) return false;
    } else if (ray.origin.z < min.z || ray.origin.z > max.z) {
        return false;
    }
    
    return t_max > 0;
}

float BoundingBox::surface_area() const {
    Vector3 d = max - min;
    return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
}

int BoundingBox::longest_axis() const {
    Vector3 d = max - min;
    if (d.x > d.y && d.x > d.z) return 0;
    if (d.y > d.z) return 1;
    return 2;
}

// KDTree Implementation
KDTree::KDTree(int max_depth, int max_triangles_per_leaf) 
    : max_depth(max_depth), max_triangles_per_leaf(max_triangles_per_leaf) {
}

void KDTree::build(const std::vector<Triangle>& triangles) {
    std::cout << "Building KD-Tree with " << triangles.size() << " triangles...\n";
    
    root = std::make_unique<KDNode>();
    
    // Zeiger auf alle Dreiecke erstellen
    std::vector<const Triangle*> triangle_ptrs;
    triangle_ptrs.reserve(triangles.size());
    for (const auto& tri : triangles) {
        triangle_ptrs.push_back(&tri);
    }
    
    // Bounding Box der gesamten Szene berechnen
    root->bbox = compute_bbox(triangle_ptrs);
    
    // Rekursiv aufbauen
    build_recursive(root.get(), triangle_ptrs, 0);
    
    std::cout << "KD-Tree built successfully!\n";
    print_stats();
}

void KDTree::build_recursive(KDNode* node, std::vector<const Triangle*>& triangles, int depth) {
    // Stopp-Kriterien
    if (depth >= max_depth || triangles.size() <= max_triangles_per_leaf) {
        node->is_leaf = true;
        node->triangles = triangles;
        return;
    }
    
    // Beste Teilungsachse finden
    int best_axis = node->bbox.longest_axis();
    float best_pos = 0.0f;
    float best_cost = 1e30f;
    
    // Verschiedene Teilungspositionen testen
    for (int axis = 0; axis < 3; axis++) {
        float min_coord = (axis == 0) ? node->bbox.min.x : 
                         (axis == 1) ? node->bbox.min.y : node->bbox.min.z;
        float max_coord = (axis == 0) ? node->bbox.max.x : 
                         (axis == 1) ? node->bbox.max.y : node->bbox.max.z;
        
        for (int i = 1; i < 4; i++) {
            float pos = min_coord + (max_coord - min_coord) * i / 4.0f;
            float cost = evaluate_split(triangles, axis, pos);
            
            if (cost < best_cost) {
                best_cost = cost;
                best_axis = axis;
                best_pos = pos;
            }
        }
    }
    
    // Dreiecke aufteilen
    std::vector<const Triangle*> left_triangles, right_triangles;
    
    for (const Triangle* tri : triangles) {
        BoundingBox tri_bbox = compute_triangle_bbox(*tri);
        float center = (best_axis == 0) ? (tri_bbox.min.x + tri_bbox.max.x) * 0.5f :
                      (best_axis == 1) ? (tri_bbox.min.y + tri_bbox.max.y) * 0.5f :
                                         (tri_bbox.min.z + tri_bbox.max.z) * 0.5f;
        
        if (center < best_pos) {
            left_triangles.push_back(tri);
        } else {
            right_triangles.push_back(tri);
        }
    }
    
    // Falls schlechte Teilung, als Blatt behandeln
    if (left_triangles.empty() || right_triangles.empty()) {
        node->is_leaf = true;
        node->triangles = triangles;
        return;
    }
    
    // Kindknoten erstellen
    node->is_leaf = false;
    node->axis = best_axis;
    node->split_pos = best_pos;
    
    node->left = std::make_unique<KDNode>();
    node->right = std::make_unique<KDNode>();
    
    // Bounding Boxes für Kindknoten berechnen
    node->left->bbox = node->bbox;
    node->right->bbox = node->bbox;
    
    if (best_axis == 0) {
        node->left->bbox.max.x = best_pos;
        node->right->bbox.min.x = best_pos;
    } else if (best_axis == 1) {
        node->left->bbox.max.y = best_pos;
        node->right->bbox.min.y = best_pos;
    } else {
        node->left->bbox.max.z = best_pos;
        node->right->bbox.min.z = best_pos;
    }
    
    // Rekursiv weiterbauen
    build_recursive(node->left.get(), left_triangles, depth + 1);
    build_recursive(node->right.get(), right_triangles, depth + 1);
}

BoundingBox KDTree::compute_bbox(const std::vector<const Triangle*>& triangles) const {
    BoundingBox bbox;
    for (const Triangle* tri : triangles) {
        bbox.expand(tri->v0);
        bbox.expand(tri->v1);
        bbox.expand(tri->v2);
    }
    return bbox;
}

BoundingBox KDTree::compute_triangle_bbox(const Triangle& tri) const {
    BoundingBox bbox;
    bbox.expand(tri.v0);
    bbox.expand(tri.v1);
    bbox.expand(tri.v2);
    return bbox;
}

float KDTree::evaluate_split(const std::vector<const Triangle*>& triangles, int axis, float pos) const {
    int left_count = 0, right_count = 0;
    
    for (const Triangle* tri : triangles) {
        BoundingBox tri_bbox = compute_triangle_bbox(*tri);
        float center = (axis == 0) ? (tri_bbox.min.x + tri_bbox.max.x) * 0.5f :
                      (axis == 1) ? (tri_bbox.min.y + tri_bbox.max.y) * 0.5f :
                                     (tri_bbox.min.z + tri_bbox.max.z) * 0.5f;
        
        if (center < pos) {
            left_count++;
        } else {
            right_count++;
        }
    }
    
    // Einfache Kostenfunktion: Anzahl der Dreiecke pro Seite
    return left_count + right_count;
}

bool KDTree::intersect(const Ray& ray, float& t, const Triangle*& hit_triangle) const {
    if (!root) return false;
    
    float min_t = 1e30f;
    const Triangle* closest_triangle = nullptr;
    
    if (intersect_recursive(root.get(), ray, min_t, closest_triangle)) {
        t = min_t;
        hit_triangle = closest_triangle;
        return true;
    }
    
    return false;
}

bool KDTree::intersect_recursive(const KDNode* node, const Ray& ray, float& min_t, const Triangle*& hit_triangle) const {
    // Bounding Box Test
    float t_min, t_max;
    if (!node->bbox.intersect(ray, t_min, t_max) || t_min > min_t) {
        return false;
    }
    
    bool hit = false;
    
    if (node->is_leaf) {
        // Blatt: Alle Dreiecke testen
        for (const Triangle* tri : node->triangles) {
            float t;
            if (tri->intersect(ray, t) && t < min_t && t > 0.001f) {
                min_t = t;
                hit_triangle = tri;
                hit = true;
            }
        }
    } else {
        // Innerer Knoten: Beide Kinder testen
        if (intersect_recursive(node->left.get(), ray, min_t, hit_triangle)) {
            hit = true;
        }
        if (intersect_recursive(node->right.get(), ray, min_t, hit_triangle)) {
            hit = true;
        }
    }
    
    return hit;
}

void KDTree::print_stats() const {
    if (!root) return;
    
    int leaf_count = 0;
    int total_triangles = 0;
    print_stats_recursive(root.get(), 0, leaf_count, total_triangles);
    
    std::cout << "KD-Tree Statistics:\n";
    std::cout << "  Leaf nodes: " << leaf_count << "\n";
    std::cout << "  Total triangles in leaves: " << total_triangles << "\n";
    std::cout << "  Average triangles per leaf: " << (float)total_triangles / leaf_count << "\n";
}

void KDTree::print_stats_recursive(const KDNode* node, int depth, int& leaf_count, int& total_triangles) const {
    if (node->is_leaf) {
        leaf_count++;
        total_triangles += node->triangles.size();
    } else {
        if (node->left) print_stats_recursive(node->left.get(), depth + 1, leaf_count, total_triangles);
        if (node->right) print_stats_recursive(node->right.get(), depth + 1, leaf_count, total_triangles);
    }
}
