// Camera.cpp
#include "Camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera(const Vector3& position, const Vector3& lookAt, const Vector3& up, double fov, double aspectRatio)
    : position(position), lookAt(lookAt), up(up), fov(fov), aspectRatio(aspectRatio) {

    // Compute camera basis vectors
    w = (lookAt - position).normalize();        // Viewing direction
    u = (w.cross(up)).normalize();              // Right vector
    v = u.cross(w);                             // Up vector in camera space

    // Convert FOV from degrees to radians
    double theta = fov * M_PI / 180.0;
    double halfHeight = tan(theta / 2.0);
    double halfWidth = aspectRatio * halfHeight;

    // Compute the lower left corner of the image plane
    lowerLeftCorner = position - u * halfWidth - v * halfHeight + w;

    // Compute the horizontal and vertical spans of the image plane
    horizontal = u * 2.0 * halfWidth;
    vertical = v * 2.0 * halfHeight;
}

Ray Camera::getRay(double s, double t) const {
    // Compute the point on the image plane
    Vector3 imagePoint = lowerLeftCorner + horizontal * s + vertical * t;
    Vector3 rayDirection = imagePoint - position;

    // Return the ray from the camera position to the image point
    return Ray(position, rayDirection.normalize());
}
