// Cylinder.cpp
#include "Cylinder.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Cylinder::Cylinder(const Vector3& baseCenter, const Vector3& axis, double radius, double height, const Material& material, bool hasCaps)
    : baseCenter(baseCenter), axis(axis.normalize()), radius(radius), height(height),  Intersectable(material), hasCaps(hasCaps) {}

bool Cylinder::intersect(const Ray& ray, HitRecord& hitRecord) const {
    // Compute the vector from the ray origin to the base center
    Vector3 oc = ray.origin - baseCenter;

    // Project vectors onto plane perpendicular to the cylinder axis
    Vector3 axis = this->axis;
    Vector3 d = ray.direction - axis * ray.direction.dot(axis);
    Vector3 oc_proj = oc - axis * oc.dot(axis);

    double a = d.dot(d);
    double b = 2.0 * d.dot(oc_proj);
    double c = oc_proj.dot(oc_proj) - radius * radius;

    double discriminant = b * b - 4 * a * c;

    double t = INFINITY;
    Vector3 normal;
    bool hit = false;

    // Check intersection with the cylindrical surface
    if (discriminant >= 0) {
        double sqrtDisc = std::sqrt(discriminant);
        double t0 = (-b - sqrtDisc) / (2 * a);
        double t1 = (-b + sqrtDisc) / (2 * a);

        // Swap if necessary
        if (t0 > t1) std::swap(t0, t1);

        // Check t0
        double y0 = (ray.origin + ray.direction * t0 - baseCenter).dot(axis);
        if (t0 >= 0 && y0 >= 0 && y0 <= height) {
            t = t0;
            Vector3 p = ray.at(t);
            Vector3 n = (p - baseCenter - axis * y0).normalize();
            normal = n;
            hit = true;
        } else {
            // Check t1
            double y1 = (ray.origin + ray.direction * t1 - baseCenter).dot(axis);
            if (t1 >= 0 && y1 >= 0 && y1 <= height) {
                t = t1;
                Vector3 p = ray.at(t);
                Vector3 n = (p - baseCenter - axis * y1).normalize();
                normal = n;
                hit = true;
            }
        }
    }

    // Check intersection with the caps if necessary
    if (hasCaps) {
        // Bottom cap
        double t_cap_bottom = (baseCenter - ray.origin).dot(axis) / ray.direction.dot(axis);
        if (t_cap_bottom >= 0) {
            Vector3 p = ray.at(t_cap_bottom);
            Vector3 d = p - baseCenter;
            if (d.dot(d) <= radius * radius) {
                if (t_cap_bottom < t) {
                    t = t_cap_bottom;
                    normal = -axis;
                    hit = true;
                }
            }
        }

        // Top cap
        Vector3 topCenter = baseCenter + axis * height;
        double t_cap_top = (topCenter - ray.origin).dot(axis) / ray.direction.dot(axis);
        if (t_cap_top >= 0) {
            Vector3 p = ray.at(t_cap_top);
            Vector3 d = p - topCenter;
            if (d.dot(d) <= radius * radius) {
                if (t_cap_top < t) {
                    t = t_cap_top;
                    normal = axis;
                    hit = true;
                }
            }
        }
    }

    if (hit) {
        hitRecord.t = t;
        hitRecord.point = ray.at(t);
        hitRecord.normal = normal;
        hitRecord.material = material;
        // Set the getUV function
        hitRecord.getUV = [this](const Vector3& point, double& u, double& v) {
            this->getUV(point, u, v);
        };
        return true;
    }

    return false;
}

void Cylinder::getUV(const Vector3& point, double& u, double& v) const {
    // Compute the vector from the base center to the point
    Vector3 p = point - baseCenter;

    // Project p onto the plane perpendicular to the axis
    double y = p.dot(axis);

    Vector3 p_proj = p - axis * y;

    // Compute the angle around the cylinder axis
    double theta = atan2(p_proj.z, p_proj.x);
    if (theta < 0)
        theta += 2 * M_PI;

    // Compute U and V
    u = theta / (2 * M_PI);
    v = y / height;

    // Ensure V is between 0 and 1
    v = std::clamp(v, 0.0, 1.0);
}