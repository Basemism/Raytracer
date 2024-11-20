// Scene.cpp

#include <vector>
#include <queue>

#include "Scene.h"
#include "Intersectable.h"
#include "Light.h"
#include "Vector3.h"

// Initialize scene with background color
Scene::Scene(const Vector3& backgroundColor) : backgroundColor(backgroundColor) {}

// Add object to scene
void Scene::addObject(std::shared_ptr<Intersectable> object) {
    objects.push_back(object);
}

// Add light to scene
void Scene::addLight(std::shared_ptr<Light> light) {
    lights.push_back(light);
}

// Pre-BVH implementation
// bool Scene::intersect(const Ray& ray, HitRecord& hitRecord) const {
//     bool hitAnything = false;
//     double closestSoFar = std::numeric_limits<double>::max();
//     HitRecord tempRecord;

//     for (const auto& object : objects) {
//         if (object->intersect(ray, tempRecord) && tempRecord.t < closestSoFar) {
//             hitAnything = true;
//             closestSoFar = tempRecord.t;
//             hitRecord = tempRecord;
//         }
//     }
    
//     return hitAnything;
// }

void Scene::buildBVH() {
    bvhRoot = std::make_shared<BVHNode>(objects, 0, objects.size());
    
    // std::cout << "BVH Tree: " << std::endl;    
    // std::cout << "Root: " << bvhRoot->boundingBox.min << " " << bvhRoot->boundingBox.max << std::endl;
    // std::queue<std::shared_ptr<BVHNode>> q;
    // Print tree using BFS.
    // q.push(bvhRoot);
    // while (!q.empty()) {
    //     auto node = q.front();
    //     q.pop();
    //     if (node->object) {
    //         std::cout << "Leaf: " << node->boundingBox.min << " " << node->boundingBox.max << std::endl;
    //     } else {
    //         std::cout << "Node: " << node->boundingBox.min << " " << node->boundingBox.max << std::endl;
    //         q.push(std::static_pointer_cast<BVHNode>(node->left));
    //         q.push(std::static_pointer_cast<BVHNode>(node->right));
    //     }
    // }    
    
}

bool Scene::intersect(const Ray& ray, HitRecord& hitRecord) const {
    if (bvhRoot) 
        return bvhRoot->intersect(ray, hitRecord);
    else {
        // Fallback to linear traversal if BVH is not built
        bool hitAnything = false;
        double closestSoFar = std::numeric_limits<double>::max();
        HitRecord tempRecord;

        for (const auto& object : objects) {
            if (object->intersect(ray, tempRecord) && tempRecord.t < closestSoFar) {
                hitAnything = true;
                closestSoFar = tempRecord.t;
                hitRecord = tempRecord;
            }
        }
        return hitAnything;
    }
}
