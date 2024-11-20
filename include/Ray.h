// Ray.h
#pragma once
#ifndef RAY_H
#define RAY_H

#include "Vector3.h"

/**
 * @brief A class representing a ray with an origin and direction.
 */
class Ray {
public:
    Vector3 origin;
    Vector3 direction;

    // Constructor
    Ray(const Vector3& origin, const Vector3& direction);

    // Compute a point along the ray at parameter t
    Vector3 at(double t) const;
};

#endif // RAY_H
