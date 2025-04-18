// RayTracer.h
#pragma once
#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Scene.h"
#include "Camera.h"
#include <random>

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
    void writeImageToPPM(const std::string& filename, const std::vector<std::vector<Vector3>>& buffer);
    void setExposure(double e);
    void setMaxDepth(int depth);
    void setRenderMode(RenderMode mode);
    void setToneMap(ToneMapping map);
    void setPixelSample(int n);
    void setLightSample(int n);
    int getPixelSamples() const { return pixelSamples; }
    int getLightSamples() const { return lightSamples; }

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
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
    int pixelSamples;
    int lightSamples;
    

    Vector3 traceRay(const Ray& ray,  int depth);
    Vector3 traceRayPath(const Ray& ray, int depth);
    Vector3 computeShadingPhong(const HitRecord& hitRecord, const Ray& ray, int depth);
    Vector3 computeShadingBin();
    Vector3 estimateDirectLight(const HitRecord& hitRecord, const Vector3& viewDir);
};

#endif // RAYTRACER_H