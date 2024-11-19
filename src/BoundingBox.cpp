// BoundingBox.cpp
#include "BoundingBox.h"
#include <algorithm>

BoundingBox::BoundingBox() : min(Vector3()), max(Vector3()) {}

BoundingBox::BoundingBox(const Vector3& min_, const Vector3& max_)
    : min(min_), max(max_) {}

BoundingBox BoundingBox::merge(const BoundingBox& other) const {
    Vector3 newMin(
        std::min(min.x, other.min.x),
        std::min(min.y, other.min.y),
        std::min(min.z, other.min.z)
    );
    Vector3 newMax(
        std::max(max.x, other.max.x),
        std::max(max.y, other.max.y),
        std::max(max.z, other.max.z)
    );
    return BoundingBox(newMin, newMax);
}

bool BoundingBox::intersect(const Ray& ray, double& tNear, double& tFar) const {
    double tmin = (min.x - ray.origin.x) / ray.direction.x;
    double tmax = (max.x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    double tymin = (min.y - ray.origin.y) / ray.direction.y;
    double tymax = (max.y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    double tzmin = (min.z - ray.origin.z) / ray.direction.z;
    double tzmax = (max.z - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    tNear = tmin;
    tFar = tmax;

    return tmax > 0;
}

Vector3 BoundingBox::getCenter() const {
    return (min + max) * 0.5;
}
