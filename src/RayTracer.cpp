// RayTracer.cpp
#include "Camera.h"
#include "Scene.h"
#include "RayTracer.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Cylinder.h"
#include "Plane.h"    // Include Plane class
#include "Light.h"
#include <iostream>
#include <memory>
#include <fstream>
#include <cmath>
#include <limits>
#include <algorithm>

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

// Compute Blinn-Phong shading at the hit point
Vector3 RayTracer::computeShading(const HitRecord& hitRecord, const Ray& ray, int depth) {
    Vector3 color(0, 0, 0);
    Vector3 viewDir = -ray.direction.normalize();

    // Ambient component
    color += hitRecord.material.ambient;

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
            Vector3 diffuse = hitRecord.material.diffuse * diff;

            // Specular component
            Vector3 halfDir = (lightDir + viewDir).normalize();
            double specAngle = std::max(0.0, hitRecord.normal.dot(halfDir));
            double spec = std::pow(specAngle, hitRecord.material.shininess);
            Vector3 specular = hitRecord.material.specular * spec;

            // Attenuate light intensity (optional)
            Vector3 lightIntensity = light.intensity;

            // Sum up the components
            color += (diffuse + specular) * lightIntensity;
        }
    }

    // **Reflection Component**
    if (hitRecord.material.reflectivity > 0.0) {
        Vector3 reflectDir = ray.direction - hitRecord.normal * 2.0 * ray.direction.dot(hitRecord.normal);
        reflectDir = reflectDir.normalize();
        Ray reflectRay(hitRecord.point + hitRecord.normal * shadowBias, reflectDir);
        Vector3 reflectColor = traceRay(reflectRay, depth + 1);
        color += reflectColor * hitRecord.material.reflectivity;
    }

    return color;
}

void RayTracer::setExposure(double e) {
    exposure = e;
}

int main() {
    // Image dimensions
    int imageWidth = 800;
    int imageHeight = 600;
    double aspectRatio = double(imageWidth) / imageHeight;

    // Create camera
    Vector3 cameraPosition(0, 5, 10);
    Vector3 cameraLookAt(0, 0, 0);
    Vector3 cameraUp(0, 1, 0);
    double fov = 45.0; // Field of view in degrees
    Camera camera(cameraPosition, cameraLookAt, cameraUp, fov, aspectRatio);

    // Create scene
    Vector3 backgroundColor(0.2, 0.7, 0.8); // Light blue background
    Scene scene(backgroundColor);

    // Materials
    Material redMaterial(
        Vector3(0.1, 0.0, 0.0),   // Ambient
        Vector3(0.7, 0.0, 0.0),   // Diffuse
        Vector3(1.0, 1.0, 1.0),   // Specular
        32,                       // Shininess
        0.5                       // Reflectivity
    );

    Material greenMaterial(
        Vector3(0.0, 0.1, 0.0),
        Vector3(0.0, 0.7, 0.0),
        Vector3(1.0, 1.0, 1.0),
        32,
        0.3 // Less reflective
    );

    Material blueMaterial(
        Vector3(0.0, 0.0, 0.1),
        Vector3(0.0, 0.0, 0.7),
        Vector3(1.0, 1.0, 1.0),
        32,
        0.0 // Non-reflective
    );

    Material grayMaterial(
        Vector3(0.2, 0.2, 0.2), 
        Vector3(0.5, 0.5, 0.5),
        Vector3(0.0, 0.0, 0.0), 
        1,
        1);

    Material mirrorMaterial(
        Vector3(0.0, 0.0, 0.0), // Ambient (black, since mirror doesn't have ambient color)
        Vector3(0.0, 0.0, 0.0), // Diffuse (black, mirror reflects rather than diffuses)
        Vector3(1.0, 1.0, 1.0), // Specular (white for mirror-like reflection)
        64,                     // Shininess (high value)
        1.0                     // Reflectivity (fully reflective)
    );

    // Add spheres to the scene
    Sphere* sphere1 = new Sphere(Vector3(-1.5, 1.0, 0), 1.0, redMaterial);
    Sphere* sphere2 = new Sphere(Vector3(1.5, 1.0, 0), 1.0, greenMaterial);
    scene.addObject(sphere1);
    scene.addObject(sphere2);

    // Add a plane to the scene (ground plane)
    Plane* groundPlane = new Plane(Vector3(0, 0, 0), Vector3(0, 1, 0), grayMaterial);
    scene.addObject(groundPlane);

    Plane* mirror = new Plane(
        Vector3(0, 0, -5),      // A point on the plane
        Vector3(0, 0, 1),       // Plane normal pointing towards the camera
        mirrorMaterial          // Mirror material
    );
    scene.addObject(mirror);

    // Add light to the scene
    Light light(Vector3(5, 10, 5), Vector3(1.0, 1.0, 1.0));
    scene.addLight(light);

    // Add a ground plane (large sphere below the objects)
    Material groundMaterial(Vector3(0.05, 0.05, 0.05), Vector3(0.5, 0.5, 0.5), Vector3(0.0, 0.0, 0.0), 1);
    Plane* ground = new Plane(Vector3(0, -5 , -5), Vector3(0, 5, 5), groundMaterial);
    scene.addObject(ground);

    // Add another light source to create more interesting shadows
    Light light2(Vector3(-5, 5, 5), Vector3(0.5, 0.5, 0.5));
    scene.addLight(light2);

    // Create ray tracer
    RayTracer rayTracer(&scene, &camera, imageWidth, imageHeight);
    rayTracer.setExposure(1); // Adjust the exposure as desired

    // Render the scene
    std::string filename = "output.ppm";
    rayTracer.render(filename);

    std::cout << "Rendering complete. Image saved to " << filename << std::endl;

    // Clean up
    delete sphere1;
    delete sphere2;
    delete groundPlane;

    return 0;
}