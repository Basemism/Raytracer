// BVHNode.cpp
#include "BVHNode.h"
#include <algorithm>

BVHNode::BVHNode() : left(nullptr), right(nullptr), object(nullptr) {}

BVHNode::BVHNode(const std::vector<std::shared_ptr<Intersectable>>& objects, size_t start, size_t end) {
    // Compute bounding box that contains all objects in this node
    BoundingBox bbox = objects[start]->getBoundingBox();
    for (size_t i = start + 1; i < end; ++i) {
        bbox = bbox.merge(objects[i]->getBoundingBox());
    }
    boundingBox = bbox;

    size_t objectSpan = end - start;

    if (objectSpan == 1) {
        // Leaf node
        object = objects[start];
        left = right = nullptr;
    } else if (objectSpan == 2) {
        // Create leaf nodes
        left = objects[start];
        right = objects[start + 1];
    } else {
        // Compute the axis with the largest extent
        Vector3 bboxSize = boundingBox.max - boundingBox.min;
        int axis = 0;
        if (bboxSize.y > bboxSize.x)
            axis = 1;
        if (bboxSize.z > bboxSize[axis])
            axis = 2;

        // Sort objects along that axis
        auto comparator = [axis](const std::shared_ptr<Intersectable>& a, const std::shared_ptr<Intersectable>& b) {
            return a->getBoundingBox().getCenter()[axis] < b->getBoundingBox().getCenter()[axis];
        };
        std::vector<std::shared_ptr<Intersectable>> sortedObjects(objects.begin() + start, objects.begin() + end);
        std::sort(sortedObjects.begin(), sortedObjects.end(), comparator);

        size_t mid = objectSpan / 2;
        left = std::make_shared<BVHNode>(sortedObjects, 0, mid);
        right = std::make_shared<BVHNode>(sortedObjects, mid, sortedObjects.size());
    }
}

bool BVHNode::intersect(const Ray& ray, HitRecord& hitRecord) const {
    double tNear, tFar;
    if (!boundingBox.intersect(ray, tNear, tFar))
        return false;

    bool hitLeft = false, hitRight = false;
    HitRecord leftRecord, rightRecord;

    if (object) {
        // Leaf node
        return object->intersect(ray, hitRecord);
    } else {
        if (left)
            hitLeft = left->intersect(ray, leftRecord);
        if (right)
            hitRight = right->intersect(ray, rightRecord);

        if (hitLeft && hitRight) {
            if (leftRecord.t < rightRecord.t) {
                hitRecord = leftRecord;
            } else {
                hitRecord = rightRecord;
            }
            return true;
        } else if (hitLeft) {
            hitRecord = leftRecord;
            return true;
        } else if (hitRight) {
            hitRecord = rightRecord;
            return true;
        } else {
            return false;
        }
    }
}

BoundingBox BVHNode::getBoundingBox() const {
    return boundingBox;
}