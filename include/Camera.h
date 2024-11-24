// Camera.h
#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <random>

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

        // New members for lens sampling
    double aperture;      // Diameter of the lens aperture
    double focusDist;     // Focus distance (distance to the focal plane)
    double lensRadius;    // Radius of the lens aperture (aperture / 2)


    // Constructor
    Camera(const Vector3& position, const Vector3& lookAt, const Vector3& up,
           double fov, double aspectRatio,
           double aperture = 0.0, double focusDist = 1.0);

    // Generate a ray through the pixel at (s, t)
    Ray getRay(double s, double t, bool useLens = false) const;

    Vector3 randomInUnitDisk() const {
        std::uniform_real_distribution<double> dist(-1.0, 1.0);
        std::mt19937 rng = std::mt19937(std::random_device{}());
        double x, y;
        do {
            x = dist(rng);
            y = dist(rng);
        } while (x * x + y * y >= 1.0);
        return Vector3(x, y, 0);
    }
};

#endif // CAMERA_H
