// BoundingBox.h
#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Vector3.h"
#include "Ray.h"

class BoundingBox {
public:
    Vector3 min;
    Vector3 max;

    BoundingBox();
    BoundingBox(const Vector3& min_, const Vector3& max_);

    BoundingBox merge(const BoundingBox& other) const;
    bool intersect(const Ray& ray, double& tNear, double& tFar) const;
    Vector3 getCenter() const;
};

#endif // BOUNDINGBOX_H
