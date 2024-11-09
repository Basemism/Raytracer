// RayTracer.cpp
#include "Camera.h"
#include "Scene.h"
#include "RayTracer.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Cylinder.h"
#include "Light.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <memory>
#include <fstream>
#include <cmath>
#include <limits>
#include <algorithm>

using json = nlohmann::json;

Scene parseSceneSettings(const json& sceneJson, int& maxDepth, std::string& renderMode, Vector3& backgroundColor);
Camera parseCamera(const json& cameraJson, int& imageWidth, int& imageHeight, double& exposure);
void parseLights(const json& lightsJson, Scene& scene);
void parseShapes(const json& shapesJson, Scene& scene, std::vector<std::shared_ptr<Intersectable>>& objects);
Material parseMaterial(const json& materialJson);

RayTracer::RayTracer(Scene* scene, Camera* camera, int imageWidth, int imageHeight)
    : scene(scene), camera(camera), imageWidth(imageWidth), imageHeight(imageHeight) {}


/* 
* Main function to parse the JSON file and render the scene.
*/
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

    // Variables to hold parsed data
    int maxDepth = 5;
    Vector3 backgroundColor(0, 0, 0);
    int imageWidth = 800;
    int imageHeight = 600;
    double exposure = 1.0;
    std::string renderModeStr;

    // Parse scene settings
    Scene scene = parseSceneSettings(sceneJson, maxDepth, renderModeStr, backgroundColor);

    // Parse camera settings
    Camera camera = parseCamera(sceneJson["camera"], imageWidth, imageHeight, exposure);

    // Parse lights
    parseLights(sceneJson["scene"]["lightsources"], scene);

    // Parse shapes
    std::vector<std::shared_ptr<Intersectable>> objects;
    parseShapes(sceneJson["scene"]["shapes"], scene, objects);

    // iterate over objects and print their material properties
    // for (const auto& object : objects) {

    //     std::cout << "Material properties for object:\n";
    //     std::cout << "ks: " << object->material.ks << "\n";
    //     std::cout << "kd: " << object->material.kd << "\n";
    //     std::cout << "isReflective: " << object->material.isReflective << "\n";
    //     std::cout << "reflectivity: " << object->material.reflectivity << "\n";
    // }

    RayTracer rayTracer(&scene, &camera, imageWidth, imageHeight);

    RayTracer::RenderMode renderModeEnum;
    if (renderModeStr == "phong") {
        renderModeEnum = RayTracer::PHONG;
    } else if (renderModeStr == "binary") {
        renderModeEnum = RayTracer::BINARY;
    } else {
        std::cerr << "Error: Unsupported rendermode '" << renderModeStr << "'. Defaulting to 'phong'." << std::endl;
        renderModeEnum = RayTracer::PHONG;
    }

    rayTracer.setRenderMode(renderModeEnum);

    // Set exposure and max recursion depth
    rayTracer.setExposure(exposure);
    rayTracer.setMaxDepth(maxDepth);

    // Render the scene
    rayTracer.render(outputFilename);

    std::cout << "Rendering complete. Image saved to " << outputFilename << std::endl;

    return 0;
}

/* 
* Function to render the scene and output to a PPM file.
*/
void RayTracer::render(const std::string& filename) {
    // std::cout << imageWidth << " X " << imageHeight << std::endl;
    std::ofstream outFile(filename);
    outFile << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

    // Loop over each pixel
    for (int j = imageHeight - 1; j >= 0; --j) {
        for (int i = 0; i < imageWidth; ++i) {
            double u = 1.0 - (double(i) / (imageWidth - 1));
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


/* 
* Function to trace a ray and compute the shading.
*/
Vector3 RayTracer::traceRay(const Ray& ray, int depth) {
    if (depth >= maxDepth) {
        // Terminate recursion
        return scene->backgroundColor;
    }

    HitRecord hitRecord;
    if (scene->intersect(ray, hitRecord)) {
        if (renderMode == PHONG) {
            return computeShadingPhong(hitRecord, ray, depth);
        } else if (renderMode == BINARY) {
            return computeShadingBin();
        } else {
            // Default to background color if render mode is unrecognized
            return scene->backgroundColor;
        }
    } else {
        // Return background color
        if (renderMode == BINARY) {
            return Vector3(0, 0, 0);
        }
        return scene->backgroundColor;
    }
}

/* 
* Function to compute the Fresnel reflectance.
*/
double fresnelReflectance(double cosTheta, double refractiveIndex) {
    double r0 = (1.0 - refractiveIndex) / (1.0 + refractiveIndex);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

/*
* Function to compute Phong shading.
*/
Vector3 RayTracer::computeShadingPhong(const HitRecord& hitRecord, const Ray& ray, int depth) {
    // Ambient component
    double ambientIntensity = 0.25;
    Vector3 ambientColor = hitRecord.material.diffuseColor * ambientIntensity;

    // Initialize diffuse and specular components
    Vector3 diffuseColor(0, 0, 0);
    Vector3 specularColor(0, 0, 0);

    // View direction (from hit point to camera)
    Vector3 viewDir = -ray.direction.normalize();

    // Iterate over each light source
    for (const auto& light : scene->lights) {
        Vector3 lightDir = (light.position - hitRecord.point).normalize();
        Vector3 halfVector = (lightDir + viewDir).normalize();

        // Shadow check
        Ray shadowRay(hitRecord.point + hitRecord.normal * shadowBias, lightDir);
        HitRecord shadowHit;
        bool inShadow = false;
        if (scene->intersect(shadowRay, shadowHit)) {
            double lightDistance = (light.position - hitRecord.point).length();
            if (shadowHit.t < lightDistance) {
                inShadow = true;
            }
        }

        if (!inShadow) {
            // Diffuse shading (Lambertian)
            double diffuseFactor = std::max(0.0, hitRecord.normal.dot(lightDir));
            diffuseColor += hitRecord.material.diffuseColor * hitRecord.material.kd * diffuseFactor * light.intensity;

            // Specular shading (Blinn-Phong)
            double specularFactor = pow(std::max(0.0, hitRecord.normal.dot(halfVector)), hitRecord.material.specularExponent);
            specularColor += hitRecord.material.specularColor * hitRecord.material.ks * specularFactor * light.intensity;
        }
    }

    // Combine ambient, diffuse, and specular components
    Vector3 localColor = ambientColor + diffuseColor + specularColor;

    // Recursive reflection
    if (hitRecord.material.isReflective) {
        Vector3 reflectedDir = ray.direction - hitRecord.normal * 2 * ray.direction.dot(hitRecord.normal);
        Ray reflectedRay(hitRecord.point + hitRecord.normal * shadowBias, reflectedDir);
        Vector3 reflectedColor = traceRay(reflectedRay, depth + 1);
        localColor = localColor * (1 - hitRecord.material.reflectivity) + reflectedColor * hitRecord.material.reflectivity;
    }

    // Recursive refraction with Fresnel reflection
    // Recursive refraction with Fresnel mixing
    if (hitRecord.material.isRefractive && depth < maxDepth) {
        double n1 = 1.0;  // Assume air's refractive index is 1
        double n2 = hitRecord.material.refractiveIndex;
        Vector3 normal = hitRecord.normal;

        // Flip normal if the ray is exiting the object
        if (ray.direction.dot(normal) > 0.0) {
            normal = -normal;
            std::swap(n1, n2);
        }

        double eta = n1 / n2;
        double cosI = -normal.dot(ray.direction);
        double sinT2 = eta * eta * (1.0 - cosI * cosI);

        // Check for total internal reflection
        if (sinT2 <= 1.0) {
            double cosT = std::sqrt(1.0 - sinT2);
            Vector3 refractDir = ray.direction * eta + normal * (eta * cosI - cosT);
            refractDir = refractDir.normalize();

            // Fresnel reflectance calculation
            double reflectance = fresnelReflectance(cosI, n2);

            // Generate refracted ray
            Ray refractRay(hitRecord.point - normal * shadowBias, refractDir);
            Vector3 refractColor = traceRay(refractRay, depth + 1);

            // Generate reflected ray
            Vector3 reflectDir = ray.direction - normal * 2.0 * ray.direction.dot(normal);
            Ray reflectRay(hitRecord.point + normal * shadowBias, reflectDir);
            Vector3 reflectColor = traceRay(reflectRay, depth + 1);

            // Mix reflection and refraction based on Fresnel coefficient
            localColor = reflectColor * reflectance + refractColor * (1.0 - reflectance);
        }
    }


    return localColor;
}

/*
* Function to compute binary shading.
*/
Vector3 RayTracer::computeShadingBin() {
    // Return red color on hit
    return Vector3(1.0, 0.0, 0.0);
}

/*
* Function to parse the scene settings from the JSON file.
*/
Scene parseSceneSettings(const json& sceneJson, int& maxDepth, std::string& renderMode, Vector3& backgroundColor) {
    // Parse nbounces (max recursion depth)
    maxDepth = sceneJson.value("nbounces", 5);

    // Parse rendermode (if needed)
    renderMode = sceneJson.value("rendermode", "phong");

    // Parse background color
    backgroundColor = Vector3(
        sceneJson["scene"]["backgroundcolor"][0],
        sceneJson["scene"]["backgroundcolor"][1],
        sceneJson["scene"]["backgroundcolor"][2]
    );

    return Scene(backgroundColor);
}

/*
* Function to parse the camera settings from the JSON file.
*/
Camera parseCamera(const json& cameraJson, int& imageWidth, int& imageHeight, double& exposure) {
    imageWidth = cameraJson["width"];
    imageHeight = cameraJson["height"];
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
    exposure = cameraJson.value("exposure", 1.0);

    return Camera(cameraPosition, cameraLookAt, cameraUp, fov, aspectRatio);
}

/*
* Function to parse the light sources from the JSON file.
*/
void parseLights(const json& lightsJson, Scene& scene) {
    for (const auto& lightJson : lightsJson) {
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
}

/* 
* Function to parse the shapes from the JSON file.
*/
void parseShapes(const json& shapesJson, Scene& scene, std::vector<std::shared_ptr<Intersectable>>& objects) {
    for (const auto& shapeJson : shapesJson) {
        std::string shapeType = shapeJson["type"];

        // Parse material
        Material material;
        
        if(shapeJson.contains("material")) {
            material = parseMaterial(shapeJson["material"]);
        }

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
            height *= 2.0;
            baseCenter = baseCenter - axis * height / 2.0;
            axis = axis.normalize();

            auto cylinder = std::make_shared<Cylinder>(baseCenter, axis, radius, height, material);
            scene.addObject(cylinder.get());
            objects.push_back(cylinder);

        } else {
            std::cerr << "Error: Unsupported shape type '" << shapeType << "'" << std::endl;
        }
    }
}

/*
* Function to parse the material properties from the JSON file.
*/
Material parseMaterial(const json& materialJson) {
    double ks = materialJson.value("ks", 0.0);
    double kd = materialJson.value("kd", 0.0);
    int specularExponent = materialJson.value("specularexponent", 1.0);

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

    // Provide default values using .value()
    bool isReflective = materialJson.value("isreflective", false);
    double reflectivity = materialJson.value("reflectivity", 0.0);

    bool isRefractive = materialJson.value("isrefractive", false);
    double refractiveIndex = materialJson.value("refractiveindex", 1.0);

    Material material(ks, kd, specularExponent, isReflective, reflectivity, isRefractive, refractiveIndex, diffuseColor, specularColor);

    return material;
}

void RayTracer::setExposure(double e) {
    exposure = e;
}

void RayTracer::setMaxDepth(int depth) {
    maxDepth = depth;
}

void RayTracer::setRenderMode(RenderMode mode) {
    renderMode = mode;
}