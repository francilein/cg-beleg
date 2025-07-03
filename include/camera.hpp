#pragma once
#include "geometry.hpp"

struct Camera {
    Point3 eye;
    Vector3 view;
    float width, height;
    int width_px, height_px;

    Camera(Point3 eye, Vector3 view, float width, float height, int wp, int hp)
        : eye(eye), view(view.normalize()), width(width), height(height), width_px(wp), height_px(hp) {}

    Ray get_ray(int x, int y) const {
        float px = ((x + 0.5f) / width_px - 0.5f) * width;
        float py = ((y + 0.5f) / height_px - 0.5f) * height;
        Vector3 right = Vector3(1, 0, 0);  // vereinfachte Annahme
        Vector3 up = Vector3(0, 1, 0);
        Vector3 pixel_pos = eye + view + right * px + up * py;
        return Ray(eye, pixel_pos - eye);
    }
};
