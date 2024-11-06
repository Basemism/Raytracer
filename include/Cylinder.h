// Cylinder.h
#ifndef CYLINDER_H
#define CYLINDER_H

#include "Intersectable.h"

/**
 * @brief A class representing a cylinder in the scene.
 */
class Cylinder : public Intersectable {
public:
    Vector3 baseCenter;
    Vector3 axis;
    double radius;
    double height;

    // Constructor
    Cylinder(const Vector3& baseCenter, const Vector3& axis, double radius, double height, const Material& material);

    // Ray-cylinder intersection
    virtual bool intersect(const Ray& ray, HitRecord& hitRecord) const;
};

#endif // CYLINDER_H
