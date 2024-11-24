// Camera.cpp
#include "Camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera(const Vector3& position, const Vector3& lookAt, const Vector3& up,
               double fov, double aspectRatio,
               double aperture, double focusDist)
    : position(position), lookAt(lookAt), up(up), fov(fov),
      aspectRatio(aspectRatio), aperture(aperture), focusDist(focusDist) {

    lensRadius = aperture / 2.0;

    // Compute camera basis vectors
    w = (position - lookAt).normalize(); // Note: pointing from position to lookAt
    u = up.cross(w).normalize();
    v = w.cross(u);

    // Convert FOV from degrees to radians
    double theta = fov * M_PI / 180.0;
    double halfHeight = tan(theta / 2.0);
    double halfWidth = aspectRatio * halfHeight;

    // Adjust the lower left corner and spans based on focus distance
    lowerLeftCorner = position - u * halfWidth * focusDist - v * halfHeight * focusDist - w * focusDist;
    horizontal = u * 2.0 * halfWidth * focusDist;
    vertical = v * 2.0 * halfHeight * focusDist;
}

Ray Camera::getRay(double s, double t, bool useLens) const {
    // Compute the point on the image plane (focal plane)
    Vector3 rd(0, 0, 0);
    Vector3 offset(0, 0, 0);

    if (useLens && lensRadius > 0.0) {
        // Sample point on the lens aperture
        rd = randomInUnitDisk() * lensRadius;
        offset = u * rd.x + v * rd.y;
    }

    Vector3 imagePoint = lowerLeftCorner + horizontal * s + vertical * t;
    Vector3 rayDirection = imagePoint - position - offset;

    // Return the ray from the lens position to the image point
    return Ray(position + offset, rayDirection.normalize());
}




