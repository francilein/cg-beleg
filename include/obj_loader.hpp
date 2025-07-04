#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include "geometry.hpp"

inline std::vector<Triangle> load_obj(const std::string& filename, const Vector3& default_color = {255, 255, 255}) {
    std::vector<Point3> vertices;
    std::vector<Triangle> triangles;

    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Could not open .obj file: " + filename);

    std::string line;
    Vector3 current_color = default_color;
    int line_number = 0;

    try {
        while (std::getline(in, line)) {
            line_number++;
            
            // Leere Zeilen und Kommentare überspringen
            if (line.empty() || line[0] == '#') {
                // Aber trotzdem nach Farben in Kommentaren suchen
                if (line.find("# color") == 0) {
                    std::istringstream iss(line);
                    std::string hash, tag;
                    int r, g, b;
                    if (iss >> hash >> tag >> r >> g >> b) {
                        current_color = Vector3((float)r, (float)g, (float)b);
                    }
                }
                continue;
            }
            
            std::istringstream iss(line);
            std::string prefix;
            if (!(iss >> prefix)) continue;

            if (prefix == "v") {
                float x, y, z;
                if (iss >> x >> y >> z) {
                    // Prüfe auf gültige Werte
                    if (std::isnan(x) || std::isnan(y) || std::isnan(z) ||
                        std::abs(x) > 1e6f || std::abs(y) > 1e6f || std::abs(z) > 1e6f) {
                        std::cout << "Warnung: Ungültiger Vertex in Zeile " << line_number << ": (" << x << ", " << y << ", " << z << ")\n";
                        continue;
                    }
                    vertices.emplace_back(x, y, z);
                } else {
                    std::cout << "Warnung: Fehlerhafte Vertex-Zeile " << line_number << ": " << line << "\n";
                }
            }
            else if (prefix == "f") {
                std::string v1_str, v2_str, v3_str;
                if (iss >> v1_str >> v2_str >> v3_str) {
                    // Parse Vertex-Indizes (kann / enthalten für Texturen/Normalen)
                    auto parse_index = [](const std::string& str) -> int {
                        size_t slash_pos = str.find('/');
                        if (slash_pos != std::string::npos) {
                            return std::stoi(str.substr(0, slash_pos));
                        }
                        return std::stoi(str);
                    };
                    
                    try {
                        int i1 = parse_index(v1_str);
                        int i2 = parse_index(v2_str);
                        int i3 = parse_index(v3_str);
                        
                        // Indizes validieren (OBJ ist 1-basiert)
                        if (i1 >= 1 && i1 <= (int)vertices.size() &&
                            i2 >= 1 && i2 <= (int)vertices.size() &&
                            i3 >= 1 && i3 <= (int)vertices.size()) {
                            
                            triangles.emplace_back(vertices[i1 - 1], vertices[i2 - 1], vertices[i3 - 1], current_color);
                        } else {
                            std::cout << "Warnung: Ungültige Face-Indizes in Zeile " << line_number << ": " << i1 << ", " << i2 << ", " << i3 << "\n";
                        }
                    } catch (const std::exception& e) {
                        std::cout << "Warnung: Fehler beim Parsen der Face-Zeile " << line_number << ": " << line << "\n";
                    }
                } else {
                    std::cout << "Warnung: Fehlerhafte Face-Zeile " << line_number << ": " << line << "\n";
                }
            }
            // Andere Zeilen (mtllib, usemtl, o, g, s, vn, vt) ignorieren
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Fehler beim Laden der OBJ-Datei in Zeile " + std::to_string(line_number) + ": " + e.what());
    }

    std::cout << "OBJ geladen: " << vertices.size() << " Vertices, " << triangles.size() << " Dreiecke\n";
    return triangles;
}
