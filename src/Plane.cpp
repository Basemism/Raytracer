// Plane.cpp
#include "Plane.h"

// Constructor initializes the plane with a point, normal, and material
Plane::Plane(const Vector3& point, const Vector3& normal, const Material& material)
    : Intersectable(material), point(point), normal(normal.normalize()) {}

// Ray-plane intersection
bool Plane::intersect(const Ray& ray, HitRecord& hitRecord) const {
    double denom = normal.dot(ray.direction);
    if (std::abs(denom) > 1e-6) { // Avoid division by zero
        double t = (point - ray.origin).dot(normal) / denom;
        if (t >= shadowBias) { // Use shadowBias to prevent self-intersection
            hitRecord.t = t;
            hitRecord.point = ray.at(t);
            hitRecord.normal = normal;
            hitRecord.material = material;
            return true;
        }
    }
    return false;
}