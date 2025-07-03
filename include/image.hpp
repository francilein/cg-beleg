#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

struct Color {
    unsigned char r, g, b;
    Color(int r=0, int g=0, int b=0) : r(std::clamp(r, 0, 255)), g(std::clamp(g, 0, 255)), b(std::clamp(b, 0, 255)) {}
};

class Image {
    int width, height;
    std::vector<Color> pixels;
public:
    Image(int w, int h) : width(w), height(h), pixels(w*h) {}

    void set_pixel(int x, int y, const Color& c) { pixels[y*width + x] = c; }

    void save_ppm(const std::string& filename) const {
        std::ofstream out(filename);
        out << "P3\n" << width << " " << height << "\n255\n";
        for (auto& c : pixels) out << (int)c.r << " " << (int)c.g << " " << (int)c.b << " ";
        out.close();
    }
};