// BVHNode.h
#pragma once
#ifndef BVHNODE_H
#define BVHNODE_H

#include "BoundingBox.h"
#include "Intersectable.h"
#include <memory>
#include <vector>

class BVHNode : public Intersectable {
public:
    BoundingBox boundingBox;
    std::shared_ptr<Intersectable> left;
    std::shared_ptr<Intersectable> right;
    std::shared_ptr<Intersectable> object; // Only for leaf nodes

    BVHNode();
    BVHNode(const std::vector<std::shared_ptr<Intersectable>>& objects, size_t start, size_t end);
    virtual bool intersect(const Ray& ray, HitRecord& hitRecord) const override;
    virtual BoundingBox getBoundingBox() const override;

};

#endif // BVHNODE_H
