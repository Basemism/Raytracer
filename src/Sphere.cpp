// Sphere.cpp
#include "Sphere.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Initialize sphere with center, radius, and material
Sphere::Sphere(const Vector3& center, double radius, const Material& material)
    : center(center), radius(radius), material(material) {}

// Ray-sphere intersection test
// Sphere.cpp (Update the intersect method) 
bool Sphere::intersect(const Ray& ray, HitRecord& hitRecord) const {
    Vector3 oc = ray.origin - center;
    double a = ray.direction.dot(ray.direction);
    double b = 2.0 * oc.dot(ray.direction);
    double c = oc.dot(oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false;
    } else {
        double sqrtDiscriminant = std::sqrt(discriminant);
        double t0 = (-b - sqrtDiscriminant) / (2.0 * a);
        double t1 = (-b + sqrtDiscriminant) / (2.0 * a);

        // Find the nearest positive t
        double t = t0;
        if (t < 0) {
            t = t1;
            if (t < 0) {
                return false;
            }
        }

        // Fill the hit record
        hitRecord.t = t;
        hitRecord.point = ray.at(t);
        hitRecord.normal = (hitRecord.point - center).normalize();
        hitRecord.material = material;
        hitRecord.getUV = [this](const Vector3& point, double& u, double& v) {
            getUV(point, u, v);
        };

        return true;
    }
}

void Sphere::getUV(const Vector3& point, double& u, double& v) const {
    Vector3 p = (point - center).normalize();
    double phi = atan2(p.z, p.x);
    double theta = acos(p.y);

    u = (phi + M_PI) / (2 * M_PI);
    v = theta / M_PI;
}

// Sphere.cpp
BoundingBox Sphere::getBoundingBox() const {
    Vector3 radiusVec(radius, radius, radius);
    return BoundingBox(center - radiusVec, center + radiusVec);
}

