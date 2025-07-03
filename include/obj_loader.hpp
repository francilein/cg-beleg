#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "geometry.hpp"

inline std::vector<Triangle> load_obj(const std::string& filename, const Vector3& default_color = {255, 255, 255}) {
    std::vector<Point3> vertices;
    std::vector<Triangle> triangles;

    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Could not open .obj file");

    std::string line;
    Vector3 current_color = default_color;

    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        }
        else if (prefix == "#") {
            std::string tag;
            iss >> tag;
            if (tag == "color") {
                int r, g, b;
                iss >> r >> g >> b;
                current_color = Vector3((float)r, (float)g, (float)b);
            }
        }
        else if (prefix == "f") {
            int i1, i2, i3;
            iss >> i1 >> i2 >> i3;
            triangles.emplace_back(vertices[i1 - 1], vertices[i2 - 1], vertices[i3 - 1], current_color);
        }
    }

    return triangles;
}
