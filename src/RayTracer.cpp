// RayTracer.cpp
#include "Camera.h"
#include "Scene.h"
#include "RayTracer.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Cylinder.h"
#include "Plane.h"    // Include Plane class
#include "Light.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <memory>
#include <fstream>
#include <cmath>
#include <limits>
#include <algorithm>

using json = nlohmann::json;

// Initialize ray tracer with scene, camera, and image dimensions
RayTracer::RayTracer(Scene* scene, Camera* camera, int imageWidth, int imageHeight)
    : scene(scene), camera(camera), imageWidth(imageWidth), imageHeight(imageHeight) {}

// Render the scene and write to PPM file
void RayTracer::render(const std::string& filename) {
    std::ofstream outFile(filename);
    outFile << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

    // Loop over each pixel
    for (int j = imageHeight - 1; j >= 0; --j) {
        for (int i = 0; i < imageWidth; ++i) {
            double u = double(i) / (imageWidth - 1);
            double v = double(j) / (imageHeight - 1);

            Ray ray = camera->getRay(u, v);
            Vector3 color = traceRay(ray, 0);

            // Apply exposure
            color = color * exposure;

            // Clamp color values to [0,1]
            color.x = std::min(1.0, std::max(0.0, color.x));
            color.y = std::min(1.0, std::max(0.0, color.y));
            color.z = std::min(1.0, std::max(0.0, color.z));

            // Write the pixel color to file
            int ir = static_cast<int>(255.999 * color.x);
            int ig = static_cast<int>(255.999 * color.y);
            int ib = static_cast<int>(255.999 * color.z);
            outFile << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    outFile.close();
}

// Add a new method to compute the color for a ray
Vector3 RayTracer::traceRay(const Ray& ray, int depth) {
    if (depth >= maxDepth) {
        // Terminate recursion
        return Vector3(0, 0, 0);
    }

    HitRecord hitRecord;
    if (scene->intersect(ray, hitRecord)) {
        return computeShading(hitRecord, ray, depth);
    } else {
        // Return background color
        return scene->backgroundColor;
    }
}

bool refract(const Vector3& incident, const Vector3& normal, double eta, Vector3& refracted) {
    double cosi = -std::max(-1.0, std::min(1.0, incident.dot(normal)));
    double sint2 = eta * eta * (1 - cosi * cosi);
    if (sint2 > 1.0) {
        // Total internal reflection
        return false;
    }
    double cost = std::sqrt(1.0 - sint2);
    refracted = incident * eta + normal * (eta * cosi - cost);
    refracted = refracted.normalize();
    return true;
}

// Compute Blinn-Phong shading at the hit point
Vector3 RayTracer::computeShading(const HitRecord& hitRecord, const Ray& ray, int depth) {
    Vector3 color(0, 0, 0);
    Vector3 viewDir = -ray.direction.normalize();

    // For each light in the scene
    for (const auto& light : scene->lights) {
        Vector3 lightDir = (light.position - hitRecord.point).normalize();

        // Shadow Ray
        Ray shadowRay(hitRecord.point + hitRecord.normal * shadowBias, lightDir);
        HitRecord shadowHitRecord;

        // Shadow Test
        bool inShadow = false;
        if (scene->intersect(shadowRay, shadowHitRecord)) {
            double lightDistance = (light.position - hitRecord.point).length();
            if (shadowHitRecord.t < lightDistance) {
                inShadow = true;
            }
        }

        if (!inShadow) {
            // Diffuse component
            double diff = std::max(0.0, hitRecord.normal.dot(lightDir));
            Vector3 diffuse = hitRecord.material.diffuseColor * hitRecord.material.kd * diff;

            // Specular component
            Vector3 reflectDir = ( hitRecord.normal * 2 * hitRecord.normal.dot(lightDir) - lightDir).normalize();
            double specAngle = std::max(0.0, reflectDir.dot(viewDir));
            double spec = std::pow(specAngle, hitRecord.material.specularExponent);
            Vector3 specular = hitRecord.material.specularColor * hitRecord.material.ks * spec;

            // Add components to color
            color += diffuse + specular;
        }
    }

    // Reflection
    if (hitRecord.material.isReflective && depth < maxDepth) {
        Vector3 reflectDir = ray.direction - hitRecord.normal * 2.0 * ray.direction.dot(hitRecord.normal);
        reflectDir = reflectDir.normalize();
        Ray reflectRay(hitRecord.point + hitRecord.normal * shadowBias, reflectDir);
        Vector3 reflectColor = traceRay(reflectRay, depth + 1);
        color += reflectColor * hitRecord.material.reflectivity;
    }

    // Refraction
    if (hitRecord.material.isRefractive && depth < maxDepth) {
        Vector3 refractDir;
        Vector3 normal = hitRecord.normal;
        double eta = hitRecord.material.refractiveIndex;
        double cosi = -std::max(-1.0, std::min(1.0, ray.direction.dot(normal)));
        if (cosi < 0) {
            cosi = -cosi;
            normal = -normal;
            eta = 1.0 / eta;
        }
        double k = 1 - eta * eta * (1 - cosi * cosi);
        if (k >= 0) {
            refractDir = ray.direction * eta + normal * (eta * cosi - sqrt(k));
            refractDir = refractDir.normalize();
            Ray refractRay(hitRecord.point - normal * shadowBias, refractDir);
            Vector3 refractColor = traceRay(refractRay, depth + 1);
            color += refractColor;
        }
    }

    return color;
}

void RayTracer::setExposure(double e) {
    exposure = e;
}

void RayTracer::setMaxDepth(int depth) {
    maxDepth = depth;
}

int main(int argc, char* argv[]) {
    // Check command-line arguments
    if (argc != 3) {
        std::cerr << "Usage: raytracer.exe path_to_JSON output_filename.ppm" << std::endl;
        return 1;
    }

    std::string jsonFilename = argv[1];
    std::string outputFilename = argv[2];

    // Load the JSON file
    std::ifstream jsonFile(jsonFilename);
    if (!jsonFile.is_open()) {
        std::cerr << "Error: Could not open JSON file " << jsonFilename << std::endl;
        return 1;
    }

    json sceneJson;
    jsonFile >> sceneJson;

    // Parse nbounces (max recursion depth)
    int maxDepth = sceneJson.value("nbounces", 5);

    // Parse rendermode (if needed)
    std::string renderMode = sceneJson.value("rendermode", "phong");

    // Parse camera settings
    json cameraJson = sceneJson["camera"];

    int imageWidth = cameraJson["width"];
    int imageHeight = cameraJson["height"];
    double aspectRatio = static_cast<double>(imageWidth) / imageHeight;

    Vector3 cameraPosition(
        cameraJson["position"][0],
        cameraJson["position"][1],
        cameraJson["position"][2]
    );

    Vector3 cameraLookAt(
        cameraJson["lookAt"][0],
        cameraJson["lookAt"][1],
        cameraJson["lookAt"][2]
    );

    Vector3 cameraUp(
        cameraJson["upVector"][0],
        cameraJson["upVector"][1],
        cameraJson["upVector"][2]
    );

    double fov = cameraJson["fov"];
    double exposure = cameraJson.value("exposure", 1.0);

    Camera camera(cameraPosition, cameraLookAt, cameraUp, fov, aspectRatio);

    // Parse scene settings
    json sceneData = sceneJson["scene"];

    Vector3 backgroundColor(
        sceneData["backgroundcolor"][0],
        sceneData["backgroundcolor"][1],
        sceneData["backgroundcolor"][2]
    );

    Scene scene(backgroundColor);

    // Parse lights
    for (const auto& lightJson : sceneData["lightsources"]) {
        std::string lightType = lightJson["type"];
        if (lightType == "pointlight") {
            Vector3 position(
                lightJson["position"][0],
                lightJson["position"][1],
                lightJson["position"][2]
            );
            Vector3 intensity(
                lightJson["intensity"][0],
                lightJson["intensity"][1],
                lightJson["intensity"][2]
            );
            Light light(position, intensity);
            scene.addLight(light);
        } else {
            std::cerr << "Error: Unsupported light type '" << lightType << "'" << std::endl;
        }
    }

    // Parse shapes
    std::vector<std::shared_ptr<Intersectable>> objects;
    for (const auto& shapeJson : sceneData["shapes"]) {
        std::string shapeType = shapeJson["type"];

        // Parse material
        json materialJson = shapeJson["material"];

        double ks = materialJson["ks"];
        double kd = materialJson["kd"];
        double specularExponent = materialJson["specularexponent"];

        Vector3 diffuseColor(
            materialJson["diffusecolor"][0],
            materialJson["diffusecolor"][1],
            materialJson["diffusecolor"][2]
        );

        Vector3 specularColor(
            materialJson["specularcolor"][0],
            materialJson["specularcolor"][1],
            materialJson["specularcolor"][2]
        );

        bool isReflective = materialJson["isreflective"];
        double reflectivity = materialJson["reflectivity"];

        bool isRefractive = materialJson["isrefractive"];
        double refractiveIndex = materialJson["refractiveindex"];

        Material material(ks, kd, specularExponent, diffuseColor, specularColor,
                          isReflective, reflectivity, isRefractive, refractiveIndex);

        if (shapeType == "sphere") {
            Vector3 center(
                shapeJson["center"][0],
                shapeJson["center"][1],
                shapeJson["center"][2]
            );
            double radius = shapeJson["radius"];

            auto sphere = std::make_shared<Sphere>(center, radius, material);
            scene.addObject(sphere.get());
            objects.push_back(sphere);
        } else if (shapeType == "triangle") {
            Vector3 v0(
                shapeJson["v0"][0],
                shapeJson["v0"][1],
                shapeJson["v0"][2]
            );
            Vector3 v1(
                shapeJson["v1"][0],
                shapeJson["v1"][1],
                shapeJson["v1"][2]
            );
            Vector3 v2(
                shapeJson["v2"][0],
                shapeJson["v2"][1],
                shapeJson["v2"][2]
            );

            auto triangle = std::make_shared<Triangle>(v0, v1, v2, material);
            scene.addObject(triangle.get());
            objects.push_back(triangle);
        } else if (shapeType == "cylinder") {
            Vector3 baseCenter(
                shapeJson["center"][0],
                shapeJson["center"][1],
                shapeJson["center"][2]
            );
            Vector3 axis(
                shapeJson["axis"][0],
                shapeJson["axis"][1],
                shapeJson["axis"][2]
            );
            double radius = shapeJson["radius"];
            double height = shapeJson["height"];
            height *= 2;

            auto cylinder = std::make_shared<Cylinder>(baseCenter, axis, radius, height, material);
            scene.addObject(cylinder.get());
            objects.push_back(cylinder);
        } else {
            std::cerr << "Error: Unsupported shape type '" << shapeType << "'" << std::endl;
        }
    }

    // Create ray tracer
    RayTracer rayTracer(&scene, &camera, imageWidth, imageHeight);

    // Set exposure and max recursion depth
    rayTracer.setExposure(exposure);
    rayTracer.setMaxDepth(maxDepth);

    // Render the scene
    rayTracer.render(outputFilename);

    std::cout << "Rendering complete. Image saved to " << outputFilename << std::endl;

    return 0;
}
