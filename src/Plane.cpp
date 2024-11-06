// Plane.cpp
#include "Plane.h"
#include <cmath>

Plane::Plane(const Vector3& center, const Vector3& normal, double width, double height, const Material& material)
    : center(center), normal(normal.normalize()), width(width), height(height), Intersectable(material) {}

bool Plane::intersect(const Ray& ray, HitRecord& hitRecord) const {
    double denominator = normal.dot(ray.direction);
    if (std::abs(denominator) > 1e-6) { // Avoid division by zero
        double t = (center - ray.origin).dot(normal) / denominator;
        if (t >= 0) {
            Vector3 p = ray.at(t);

            // Compute local coordinate system
            Vector3 u = normal.cross(Vector3(0.0, 1.0, 0.0));
            if (u.length() < 1e-3) { // If normal is parallel to Y axis
                u = normal.cross(Vector3(1.0, 0.0, 0.0));
            }
            u = u.normalize();
            Vector3 v = normal.cross(u);

            // Transform p to plane's local coordinate system
            Vector3 d = p - center;
            double du = d.dot(u);
            double dv = d.dot(v);

            // Check if the point is within the plane's boundaries
            if (std::abs(du) <= width / 2 && std::abs(dv) <= height / 2) {
                hitRecord.t = t;
                hitRecord.point = p;
                hitRecord.normal = normal;
                hitRecord.material = material;
                return true;
            }
        }
    }
    return false;
}
