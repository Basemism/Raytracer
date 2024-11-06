// Camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

/**
 * @brief A class representing the camera in the scene.
 */
class Camera {
public:
    Vector3 position;
    Vector3 lookAt;
    Vector3 up;
    double fov; // Field of view in degrees
    double aspectRatio;

    // Precomputed basis vectors for the camera
    Vector3 u, v, w;

    // Constructor
    Camera(const Vector3& position, const Vector3& lookAt, const Vector3& up, double fov, double aspectRatio);

    // Generate a ray through the pixel at (u, v)
    Ray getRay(double s, double t) const;
};

#endif // CAMERA_H
