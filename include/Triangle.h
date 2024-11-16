// Triangle.h
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Intersectable.h"

/**
 * @brief A class representing a triangle in the scene.
 */
class Triangle : public Intersectable {
public:
    Vector3 v0, v1, v2;

    // Constructor
    Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Material& material);

    // Ray-triangle intersection
    virtual bool intersect(const Ray& ray, HitRecord& hitRecord) const override;
    void getUV(const Vector3& point, double& u, double& v) const;

private:
    Vector3 normal;
};

#endif // TRIANGLE_H
