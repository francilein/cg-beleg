#pragma once
#include "geometry.hpp"
#include <vector>
#include <memory>

struct BoundingBox
{
    Point3 min, max;

    BoundingBox() : min(1e30f, 1e30f, 1e30f), max(-1e30f, -1e30f, -1e30f) {}
    BoundingBox(const Point3 &min, const Point3 &max) : min(min), max(max) {}

    void expand(const Point3 &point);
    void expand(const BoundingBox &box);
    bool intersect(const Ray &ray, float &t_min, float &t_max) const;
    float surface_area() const;
    int longest_axis() const;
};

struct KDNode
{
    BoundingBox bbox;
    std::vector<const Triangle *> triangles;
    std::unique_ptr<KDNode> left, right;
    int axis; // 0=x, 1=y, 2=z
    float split_pos;
    bool is_leaf;

    KDNode() : axis(0), split_pos(0.0f), is_leaf(true) {}
};

class KDTree
{
private:
    std::unique_ptr<KDNode> root;
    int max_depth;
    int max_triangles_per_leaf;

    void build_recursive(KDNode *node, std::vector<const Triangle *> &triangles, int depth);
    BoundingBox compute_bbox(const std::vector<const Triangle *> &triangles) const;
    BoundingBox compute_triangle_bbox(const Triangle &tri) const;
    float evaluate_split(const std::vector<const Triangle *> &triangles, int axis, float pos) const;
    bool intersect_recursive(const KDNode *node, const Ray &ray, float &min_t, const Triangle *&hit_triangle) const;

public:
    KDTree(int max_depth = 20, int max_triangles_per_leaf = 10);
    void build(const std::vector<Triangle> &triangles);
    bool intersect(const Ray &ray, float &t, const Triangle *&hit_triangle) const;
    void print_stats() const;
    void print_stats_recursive(const KDNode *node, int depth, int &leaf_count, int &total_triangles, int &max_depth) const;
};
