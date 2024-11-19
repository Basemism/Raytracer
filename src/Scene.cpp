// Scene.cpp

#include <vector>

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
void Scene::addLight(const Light& light) {
    lights.push_back(light);
}

// Find closest intersection of ray with scene objects
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
