// Camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

class Camera {
public:
    Vector3 position;
    Vector3 lookAt;
    Vector3 up;
    double fov; // Field of view in degrees
    double aspectRatio;

    // Precomputed basis vectors and image plane parameters
    Vector3 u, v, w;
    Vector3 lowerLeftCorner;
    Vector3 horizontal;
    Vector3 vertical;

    // Constructor
    Camera(const Vector3& position, const Vector3& lookAt, const Vector3& up, double fov, double aspectRatio);

    // Generate a ray through the pixel at (s, t)
    Ray getRay(double s, double t) const;
};

#endif // CAMERA_H
