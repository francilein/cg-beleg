// ==== include/image.hpp ====
#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include "stb_image_write.h"

struct Color
{
    unsigned char r, g, b;
    Color(int r = 0, int g = 0, int b = 0)
        : r(std::max(0, std::min(255, r))),
          g(std::max(0, std::min(255, g))),
          b(std::max(0, std::min(255, b))) {}
};

class Image
{
    int width, height;
    std::vector<Color> pixels;

public:
    Image(int w, int h) : width(w), height(h), pixels(w * h) {}

    void set_pixel(int x, int y, const Color &c)
    {
        // Bildzeile invertieren (oben = y=0)
        pixels[(height - 1 - y) * width + x] = c;
    }

    void save_png(const std::string &filename) const
    {
        std::vector<unsigned char> data;
        data.reserve(width * height * 3);
        for (const auto &c : pixels)
        {
            data.push_back(c.r);
            data.push_back(c.g);
            data.push_back(c.b);
        }
        stbi_write_png(filename.c_str(), width, height, 3, data.data(), width * 3);
    }
};