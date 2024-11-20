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
    enum RenderMode { PHONG, BINARY, PATH_TRACE};
    enum ToneMapping { NONE, REINHARD, WARD, UNCHARTED2 };

    // Constructor
    RayTracer(Scene* scene, Camera* camera, int imageWidth, int imageHeight);

    // Render the scene and output to a PPM file
    void render(const std::string& filename);
    void renderPathTrace(const std::string& filename);
    void setExposure(double e);
    void setMaxDepth(int depth);
    void setRenderMode(RenderMode mode);
    void setToneMap(ToneMapping map);

private:
    Scene* scene;
    Camera* camera;
    int imageWidth;
    int imageHeight;
    double exposure = 1.0;
    int maxDepth = 5;
    double shadowBias = 1e-4;
    RenderMode renderMode = PHONG; // Default to PHONG
    ToneMapping toneMapping = NONE; // Default to NONE

    Vector3 traceRay(const Ray& ray,  int depth);
    Vector3 traceRayPath(const Ray& ray, int depth);
    Vector3 computeShadingPhong(const HitRecord& hitRecord, const Ray& ray, int depth);
    Vector3 computeShadingPathTrace(const HitRecord& hitRecord, const Ray& ray, int depth);
    Vector3 computeShadingBin();
};

#endif // RAYTRACER_H