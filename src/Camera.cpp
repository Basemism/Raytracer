// Camera.cpp
#include "Camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Initialize camera and precompute basis vectors
Camera::Camera(const Vector3& position, const Vector3& lookAt, const Vector3& up, double fov, double aspectRatio)
    : position(position), lookAt(lookAt), up(up), fov(fov), aspectRatio(aspectRatio) {
    double theta = fov * M_PI / 180.0;
    float halftheta = theta / 2;

    double halfHeight = tan(halftheta);
    double halfWidth = aspectRatio * halfHeight;

    w = (position - lookAt).normalize();
    u = up.cross(w).normalize();
    v = w.cross(u);
}

// Generate ray through pixel at (s, t)
Ray Camera::getRay(double s, double t) const {
    Vector3 rayDirection = u * (2 * s - 1) + v * (2 * t - 1) - w;
    return Ray(position, rayDirection.normalize());
}
