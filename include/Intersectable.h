// Intersectable.h
#ifndef INTERSECTABLE_H
#define INTERSECTABLE_H

#include "Ray.h"
#include "Material.h"
#include <limits>

/**
 * @brief Abstract base class for objects that can be intersected by rays.
 */

struct HitRecord {
    double t;                 // Ray parameter t at intersection
    Vector3 point;            // Intersection point
    Vector3 normal;           // Surface normal at the intersection
    Material material;        // Material of the intersected object

    HitRecord()
        : t(0.0), point(), normal(), material(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0), 0.0) {}
};

class Intersectable {
public:
    Material material;

    // Constructor
    Intersectable(const Material& material);

    // Pure virtual function for ray intersection
    virtual bool intersect(const Ray& ray, HitRecord& hitRecord) const = 0;
};

#endif // INTERSECTABLE_H
