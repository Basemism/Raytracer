// Cylinder.cpp
#include "Cylinder.h"

// Initialize cylinder with base center, axis, radius, height, and material
Cylinder::Cylinder(const Vector3& baseCenter, const Vector3& axis, double radius, double height, const Material& material)
    : Intersectable(material), baseCenter(baseCenter), axis(axis.normalize()), radius(radius), height(height) {}

// Ray-cylinder intersection (simplified infinite cylinder)
bool Cylinder::intersect(const Ray& ray, HitRecord& hitRecord) const {
    // Implementation remains similar, but fill hitRecord when an intersection occurs
    // For brevity, I'm providing an updated version focusing on infinite cylinders
    Vector3 d = ray.direction - axis * ray.direction.dot(axis);
    Vector3 deltaP = ray.origin - baseCenter - axis * (ray.origin - baseCenter).dot(axis);
    double a = d.dot(d);
    double b = 2 * d.dot(deltaP);
    double c = deltaP.dot(deltaP) - radius * radius;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
        return false;

    double sqrtDiscriminant = std::sqrt(discriminant);
    double t0 = (-b - sqrtDiscriminant) / (2 * a);
    double t1 = (-b + sqrtDiscriminant) / (2 * a);

    double t = t0;
    if (t < 0) {
        t = t1;
        if (t < 0) {
            return false;
        }
    }

    // Check if the intersection point is within the height of the cylinder
    double y = (ray.origin + ray.direction * t - baseCenter).dot(axis);
    if (y < 0 || y > height)
        return false;

    // Fill the hit record
    hitRecord.t = t;
    hitRecord.point = ray.at(t);
    Vector3 outwardNormal = (hitRecord.point - baseCenter - axis * y).normalize();
    hitRecord.normal = outwardNormal;
    hitRecord.material = material;

    return true;
}

