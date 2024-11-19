// Cylinder.h
#ifndef CYLINDER_H
#define CYLINDER_H

#include "Intersectable.h"
#include "Vector3.h"
#include "Material.h"

/**
 * @brief A class representing a finite cylinder in 3D space.
 */
class Cylinder : public Intersectable {
public:
    Vector3 baseCenter; // Center of the base
    Vector3 axis;       // Axis direction (should be normalized)
    double radius;
    double height;
    bool hasCaps;       // Whether the cylinder has top and bottom caps
    Material material;

    // Constructor
    Cylinder(const Vector3& baseCenter, const Vector3& axis, double radius, double height, const Material& material, bool hasCaps = true);

    // Intersection method
    virtual bool intersect(const Ray& ray, HitRecord& hitRecord) const override;

    virtual BoundingBox getBoundingBox() const override;

    void getUV(const Vector3& point, double& u, double& v) const;
};

#endif // CYLINDER_H
