// Scene.h
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Intersectable.h"
#include "Light.h"
#include "Vector3.h"
#include "BVHNode.h"

/**
 * @brief A class representing the entire scene, including objects and lights.
 */
class Scene {
public:
    Vector3 backgroundColor;
    std::vector<std::shared_ptr<Intersectable>> objects;
    // std::vector<Light> lights;
    std::vector<std::shared_ptr<Light>> lights;
    std::shared_ptr<BVHNode> bvhRoot;

    // Constructor
    Scene(const Vector3& backgroundColor);

    // Add objects and lights to the scene
    void addObject(std::shared_ptr<Intersectable> object);
    void addLight(std::shared_ptr<Light> light);

    // Find the closest intersection of a ray with the scene
    bool intersect(const Ray& ray, HitRecord& hitRecord) const;

    // Build the BVH
    void buildBVH();
};


#endif // SCENE_H
