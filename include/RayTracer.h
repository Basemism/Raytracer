// RayTracer.h
#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Scene.h"
#include "Camera.h"

/**
 * @brief A class responsible for rendering the scene.
 */
class RayTracer {
public:
    Scene* scene;
    Camera* camera;
    int imageWidth;
    int imageHeight;

    // Constructor
    RayTracer(Scene* scene, Camera* camera, int imageWidth, int imageHeight);

    // Render the scene and output to a PPM file
    void render(const std::string& filename);
    void setExposure(double e);
    void setMaxDepth(int depth);

private:
    // New private methods
    Vector3 traceRay(const Ray& ray,  int depth);
    Vector3 computeShading(const HitRecord& hitRecord, const Ray& ray, int depth);

    double shadowBias = 1e-5;
    double exposure = 1.0; 
    int maxDepth = 100;

};

#endif // RAYTRACER_H
