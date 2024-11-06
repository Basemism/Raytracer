// Plane.h
#ifndef PLANE_H
#define PLANE_H

#include "Intersectable.h"
#include "Vector3.h"
#include "Material.h"

/**
 * @brief A class representing a finite plane (rectangle) in 3D space.
 */
class Plane : public Intersectable {
public:
    Vector3 center;    // Center point of the plane
    Vector3 normal;    // Normal vector of the plane
    double width;      // Width of the plane
    double height;     // Height of the plane

    // Constructor
    Plane(const Vector3& center, const Vector3& normal, double width, double height, const Material& material);

    // Intersection method
    bool intersect(const Ray& ray, HitRecord& hitRecord) const override;
};

#endif // PLANE_H
