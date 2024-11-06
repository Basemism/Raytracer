// Ray.cpp
#include "Ray.h"

// Initialize ray with origin and direction
Ray::Ray(const Vector3& origin, const Vector3& direction)
    : origin(origin), direction(direction.normalize()) {}

// Compute point along the ray at parameter t
Vector3 Ray::at(double t) const {
    return origin + direction * t;
}
