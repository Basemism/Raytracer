// Scene.h
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Intersectable.h"
#include "Light.h"
#include "Vector3.h"

/**
 * @brief A class representing the entire scene, including objects and lights.
 */
class Scene {
public:
    std::vector<Intersectable*> objects;
    std::vector<Light> lights;
    Vector3 backgroundColor;

    // Constructor
    Scene(const Vector3& backgroundColor);

    // Add objects and lights to the scene
    void addObject(Intersectable* object);
    void addLight(const Light& light);

    // Find the closest intersection of a ray with the scene
    bool intersect(const Ray& ray, HitRecord& hitRecord) const;
};


#endif // SCENE_H
