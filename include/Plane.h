#ifndef PLANE_H
#define PLANE_H

#include "Intersectable.h"

/**
 * @brief A class representing an infinite plane in the scene.
 */
class Plane : public Intersectable {
public:
    Vector3 point;   // A point on the plane
    Vector3 normal;  // Normal vector of the plane

    // Constructor
    Plane(const Vector3& point, const Vector3& normal, const Material& material);

    // Ray-plane intersection
    virtual bool intersect(const Ray& ray, HitRecord& hitRecord) const override;

private:
    double shadowBias = 1e-5; // You can try increasing this value slightly
};


#endif // PLANE_H