// Sphere.h
#ifndef SPHERE_H
#define SPHERE_H

#include "Intersectable.h"

/**
 * @brief A class representing a sphere in the scene.
 */
class Sphere : public Intersectable {
public:
    Vector3 center;
    double radius;

    // Constructor
    Sphere(const Vector3& center, double radius, const Material& material);

    // Ray-sphere intersection
    virtual bool intersect(const Ray& ray, HitRecord& hitRecord) const override;
    
    void getUV(const Vector3& point, double& u, double& v) const;
};

#endif // SPHERE_H
