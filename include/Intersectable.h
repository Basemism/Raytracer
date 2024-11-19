// Intersectable.h
#ifndef INTERSECTABLE_H
#define INTERSECTABLE_H

#include "Ray.h"
#include "Material.h"
#include "BoundingBox.h"
#include <functional>
#include <limits>

/**
 * @brief Abstract base class for objects that can be intersected by rays.
 */

struct HitRecord {
    double t;                 // Ray parameter t at intersection
    Vector3 point;            // Intersection point
    Vector3 normal;           // Surface normal at the intersection
    Material material;        // Material of the intersected object
    std::function<void(const Vector3&, double&, double&)> getUV; // Function to get UV coordinates


    HitRecord()
        : t(0.0), point(), normal(), material() {}
};

class Intersectable {
public:
    // Constructor
    Intersectable();

    // Pure virtual function for ray intersection
    virtual bool intersect(const Ray& ray, HitRecord& hitRecord) const = 0;
    virtual BoundingBox getBoundingBox() const = 0;
};

#endif // INTERSECTABLE_H
