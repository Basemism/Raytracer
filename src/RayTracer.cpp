// RayTracer.cpp
#include "Camera.h"
#include "Scene.h"
#include "RayTracer.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Cylinder.h"
#include "Light.h"
#include "AreaLight.h"
#include "PointLight.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <memory>
#include <fstream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using json = nlohmann::json;

Scene parseSceneSettings(const json& sceneJson, int& maxDepth, std::string& renderMode, Vector3& backgroundColor);
Camera parseCamera(const json& cameraJson, int& imageWidth, int& imageHeight, double& exposure);
void parseLights(const json& lightsJson, Scene& scene);
void parseShapes(const json& shapesJson, Scene& scene, std::vector<std::shared_ptr<Intersectable>>& objects);
Material parseMaterial(const json& materialJson);

RayTracer::RayTracer(Scene* scene, Camera* camera, int imageWidth, int imageHeight)
    : scene(scene), camera(camera), imageWidth(imageWidth), imageHeight(imageHeight) {
        rng.seed(std::random_device()());
        dist = std::uniform_real_distribution<double>(0.0, 1.0);
    }


/* 
* Main function to parse the JSON file and render the scene.
*/
int main(int argc, char* argv[]) {
    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    // Check command-line arguments
    if (!(argc==3 || argc==4)) {
        std::cerr << "Usage: raytracer.exe path_to_JSON output_filename.ppm <optional-tonemapping>"<< std::endl;
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
    std::cout << "Parsing scene settings..." << std::endl;
    Scene scene = parseSceneSettings(sceneJson, maxDepth, renderModeStr, backgroundColor);
    std::cout << "Scene settings parsed." << std::endl;

    // Parse camera settings
    std::cout << "Parsing camera settings..." << std::endl;
    Camera camera = parseCamera(sceneJson["camera"], imageWidth, imageHeight, exposure);
    std::cout << "Camera settings parsed." << std::endl;

    // Parse lights
    std::cout << "Parsing lights..." << std::endl;
    parseLights(sceneJson["scene"]["lightsources"], scene);
    std::cout << "Lights parsed." << std::endl;

    // Parse shapes
    std::cout << "Parsing shapes..." << std::endl;
    std::vector<std::shared_ptr<Intersectable>> objects;
    parseShapes(sceneJson["scene"]["shapes"], scene, objects);
    std::cout << "Shapes parsed." << std::endl;

    // Build the BVH
    if (sceneJson.value("bvh", true)) {
        std::cout << "Building BVH..." << std::endl;
        scene.buildBVH();
        std::cout << "BVH built." << std::endl;
    }

    // Create the ray tracer
    RayTracer rayTracer(&scene, &camera, imageWidth, imageHeight);

    RayTracer::RenderMode renderModeEnum;
    if (renderModeStr == "phong")
        renderModeEnum = RayTracer::PHONG;
    else if (renderModeStr == "binary")
        renderModeEnum = RayTracer::BINARY;
    else if (renderModeStr == "pathtrace")
        renderModeEnum = RayTracer::PATH_TRACE;
    else{
        std::cerr << "Error: Unsupported rendermode '" << renderModeStr << "'. Defaulting to 'phong'." << std::endl;
        renderModeEnum = RayTracer::PHONG;
    }

    if (argc == 4) {
        std::string toneMappingStr = argv[3];
        if (toneMappingStr == "reinhard") {
            rayTracer.setToneMap(RayTracer::REINHARD);
        } else if (toneMappingStr == "ward") {
            rayTracer.setToneMap(RayTracer::WARD);
        } else if (toneMappingStr == "uncharted2") {
            rayTracer.setToneMap(RayTracer::UNCHARTED2);
        } else {
            std::cerr << "Error: Unsupported tonemapping '" << toneMappingStr << "'. Defaulting to 'none'." << std::endl;
        }
    }

    rayTracer.setRenderMode(renderModeEnum);
    rayTracer.setExposure(exposure);
    rayTracer.setMaxDepth(maxDepth);

    if (renderModeEnum == RayTracer::PATH_TRACE) {
        int nspp = sceneJson.value("pixelsample", 16);

        int nspal = sceneJson.value("lightsample", 4);

        rayTracer.setLightSample(nspp);
        rayTracer.setPixelSample(nspal);

        std::cout << "Pixel samples: " << rayTracer.getPixelSamples() << std::endl;
        std::cout << "Light samples: " << rayTracer.getLightSamples() << std::endl;
    }
    
    // Render the scene
    if (renderModeEnum == RayTracer::PATH_TRACE)
        rayTracer.renderPathTrace(outputFilename);
    else
        rayTracer.render(outputFilename);

    // Stop timer and calculate duration
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Rendering complete. Image saved to " << outputFilename << std::endl;
    std::cout << "Total execution time: " << duration.count() << " milliseconds" << std::endl;

    return 0;
}

Vector3 uncharted2_tonemap_partial(Vector3 x)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((x*(x*A+C*B)+D*E)/(x*(x*A+B)+D*F))-E/F;
}

Vector3 uncharted2_filmic(Vector3 v)
{
    float exposure_bias = 2.0f;
    Vector3 curr = uncharted2_tonemap_partial(v * exposure_bias);

    Vector3 W = Vector3(11.2f);
    Vector3 white_scale = Vector3(1.0f) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
}

Vector3 toneMap(Vector3 color, RayTracer::ToneMapping toneMapping) {
    // Reinhard tonemapping
    if (toneMapping == RayTracer::REINHARD) {
        color = color / (color + Vector3(1, 1, 1));
        return color;
    }

    // Ward tonemapping
    if (toneMapping == RayTracer::WARD) {
        color = color / (color + Vector3(1, 1, 1));
        color = color * (color * (color * 0.4 + 0.4) + 1.219) / (color * (color * 0.3 + 0.5) + 2.219);
        return color;
    }

    // Uncharted 2 tonemapping
    if (toneMapping == RayTracer::UNCHARTED2) {
        return uncharted2_filmic(color);
    }

    return color;
}
/*
* Function to render the scene and output to a PPM file.
 */
void RayTracer::render(const std::string& filename) {
    // Image buffer to store computed colors
    std::vector<std::vector<Vector3>> buffer(imageHeight, std::vector<Vector3>(imageWidth));

    // Setup OpenMP
    #pragma omp parallel
    {
        // Loop over each pixel
        #pragma omp for schedule(dynamic)
        for (int j = 0; j < imageHeight; ++j) {
            for (int i = 0; i < imageWidth; ++i) {
                double u = 1.0 - (double(i) / (imageWidth - 1));
                double v = double(j) / (imageHeight - 1);

                Ray ray = camera->getRay(u, v);
                Vector3 color = traceRay(ray, 0);

                // Apply tone mapping
                color = toneMap(color, toneMapping);

                // Apply exposure
                color = color * exposure;

                // Clamp color values to [0,1]
                color.x = std::min(1.0, std::max(0.0, color.x));
                color.y = std::min(1.0, std::max(0.0, color.y));
                color.z = std::min(1.0, std::max(0.0, color.z));

                // Store the computed color in the buffer
                buffer[j][i] = color;
            }

            // Update progress (only from master thread)
            #pragma omp critical
            {
                int progress = (j * 100) / imageHeight;
                std::cout << "\rRendering: " << progress << "% completed" << std::flush;
            }
        }
    }

    // Write the image buffer to a PPM file
    writeImageToPPM(filename, buffer);
}

/*
* Function to parse the scene settings from the JSON file.
*/
void RayTracer::renderPathTrace(const std::string& filename) {
    const int sqrt_nspp = static_cast<int>(std::sqrt(pixelSamples)); // Grid dimensions for stratified sampling

    // Image buffer to store computed colors
    std::vector<std::vector<Vector3>> buffer(imageHeight, std::vector<Vector3>(imageWidth));

    // Setup OpenMP
    #pragma omp parallel
    {
        // Loop over each pixel
        #pragma omp for schedule(dynamic) 
        for (int j = 0; j < imageHeight; ++j) {
            for (int i = 0; i < imageWidth; ++i) {
                Vector3 color(0, 0, 0);

                // Stratified sampling within the pixel
                for (int sy = 0; sy < sqrt_nspp; ++sy) {
                    for (int sx = 0; sx < sqrt_nspp; ++sx) {
                        // Generate random offsets within the sub-pixel grid cell using thread-local RNG
                        double r1 = (sx + dist(rng)) / sqrt_nspp;
                        double r2 = (sy + dist(rng)) / sqrt_nspp;

                        // Map to image plane coordinates
                        double u = 1.0 - (double(i) + r1) / (imageWidth - 1);
                        double v = (double(j) + r2) / (imageHeight - 1);

                        // Generate ray and trace it
                        Ray ray = camera->getRay(u, v, true);

                        color += traceRayPath(ray, 0);
                    }
                }

                //Try jittered sampling
                // for (int s = 0; s < nspp; ++s) {
                //     double uOffset = dist(rng) / imageWidth;
                //     double vOffset = dist(rng) / imageHeight;
                //     double u = 1.0 - (static_cast<double>(i) + uOffset) / (imageWidth - 1);
                //     double v = (static_cast<double>(j) + vOffset) / (imageHeight - 1);;

                //     // Generate ray and trace it
                //     Ray ray = camera->getRay(u, v, true);

                //     color += traceRayPath(ray, 0);
                // }


                color /= pixelSamples; // Average the color over all samples

                // Apply tone mapping and exposure
                color = toneMap(color, toneMapping);
                color = color * exposure;

                // Gamma correction (sRGB gamma 2.2 approximation)
                color.x = pow(color.x, 1.0 / 2.2);
                color.y = pow(color.y, 1.0 / 2.2);
                color.z = pow(color.z, 1.0 / 2.2);

                // Clamp color values to [0,1]
                color.x = std::min(1.0, std::max(0.0, color.x));
                color.y = std::min(1.0, std::max(0.0, color.y));
                color.z = std::min(1.0, std::max(0.0, color.z));

                // Store the computed color in the buffer
                buffer[j][i] = color;
            }

            // Update progress (only from master thread)
            #pragma omp critical
            {
                int progress = (j * 100) / imageHeight;
                std::cout << "\rRendering: " << progress << "% completed" << std::flush;
            }
        }
    }
    
    // if (pixelSamples <= 16) {
    //     std::cout << "\nPerforming Filter Antialiasing..." << std::endl;
    //         // // Perform Antialiasing
    //     for (int j = 0; j < imageHeight; ++j) {
    //         for (int i = 0; i < imageWidth; ++i) {
    //             Vector3 color = buffer[j][i];
    //             if (i > 0 && j > 0 && i < imageWidth - 1 && j < imageHeight - 1) {
    //                 color += buffer[j - 1][i - 1] + buffer[j - 1][i] + buffer[j - 1][i + 1];
    //                 color += buffer[j][i - 1] + buffer[j][i + 1];
    //                 color += buffer[j + 1][i - 1] + buffer[j + 1][i] + buffer[j + 1][i + 1];
    //                 color /= 9.0;
    //             }
    //             buffer[j][i] = color;
    //         }
    //     }

    // }

    // Write the image buffer to a PPM file
    writeImageToPPM(filename, buffer);
}

void RayTracer::writeImageToPPM(const std::string& filename, const std::vector<std::vector<Vector3>>& buffer) {
    std::ofstream outFile(filename);
    outFile << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

    for (int j = imageHeight - 1; j >= 0; --j) {
        for (int i = 0; i < imageWidth; ++i) {
            Vector3 color = buffer[j][i];
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
        if (renderMode == PHONG) 
            return computeShadingPhong(hitRecord, ray, depth);
        else if (renderMode == BINARY) 
            return computeShadingBin();
        else
            return scene->backgroundColor; // Default to background color
        
    } else {
        if (renderMode == BINARY)
            return Vector3(0, 0, 0);
        return scene->backgroundColor;
    }
}

/*
* Function to generate a random direction in the hemisphere.
*/
Vector3 randomInHemisphere(const Vector3& normal) {
    // Generate random numbers
    double r1 = ((double)rand() / RAND_MAX);
    double r2 = ((double)rand() / RAND_MAX);

    // Convert to spherical coordinates
    double sinTheta = sqrt(1 - r1 * r1);
    double phi = 2 * M_PI * r2;

    // Convert to Cartesian coordinates
    double x = cos(phi) * sinTheta;
    double y = r1; // Cos(theta)
    double z = sin(phi) * sinTheta;

    // Create an orthonormal basis (tangent, bitangent, normal)
    Vector3 tangent, bitangent;
    if (fabs(normal.x) > fabs(normal.y)) {
        tangent = Vector3(-normal.z, 0, normal.x).normalize();
    } else {
        tangent = Vector3(0, normal.z, -normal.y).normalize();
    }
    bitangent = normal.cross(tangent);

    // Transform the sampled direction to world space
    Vector3 direction = tangent * x + normal * y + bitangent * z;
    return direction.normalize();
}


/* 
* Pathtracing Function to refract a vector through a surface. 
*/
Vector3 refract(const Vector3& I, const Vector3& N, double eta_t, double eta_i = 1.0) {
    // I: Incident direction (normalized)
    // N: Surface normal (normalized)
    // eta_i: Refractive index of incident medium
    // eta_t: Refractive index of transmitted medium
    double cosi = -std::max(-1.0, std::min(1.0, I.dot(N)));
    if (cosi < 0) {
        // Ray is inside the medium
        return refract(I, -N, eta_i, eta_t);
    }
    double eta = eta_i / eta_t;
    double k = 1 - eta * eta * (1 - cosi * cosi);
    if (k < 0) {
        // Total internal reflection
        return Vector3(0, 0, 0);
    } else {
        return I * eta + N * (eta * cosi - sqrt(k));
    }
}

/*
* Function to compute the Fresnel reflectance.
*/
double fresnel(const Vector3& I, const Vector3& N, double eta_t, double eta_i = 1.0) {
    double cosi = std::clamp(I.dot(N), -1.0, 1.0);
    double etai = eta_i;
    double etat = eta_t;
    if (cosi > 0) {
        std::swap(etai, etat);
    }
    // Compute sini using Snell's law
    double sint = etai / etat * sqrt(std::max(0.0, 1 - cosi * cosi));
    // Total internal reflection
    if (sint >= 1) {
        return 1.0;
    } else {
        double cost = sqrt(std::max(0.0, 1 - sint * sint));
        cosi = fabs(cosi);
        double Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        double Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        return (Rs * Rs + Rp * Rp) / 2.0;
    }
}

/*
* Pathtracing Function to reflect a vector about a normal.
*/
Vector3 reflect(const Vector3& v, const Vector3& n) {
    if (v.dot(n) < 0)
        return v - n * 2 * v.dot(n); // Incoming ray is inside the object (flip normal)
    return v - n * 2 * v.dot(n) ;
}

/*
* Function to refract a vector through a surface.
*/
bool shouldTerminate(const Vector3& albedo, int depth) {
    if (depth < 5) {
        return false;
    }
    double maxComponent = std::max(albedo.x, std::max(albedo.y, albedo.z));
    double terminationProbability = 1.0 - maxComponent;
    double randomValue = ((double)rand() / RAND_MAX);
    return randomValue < terminationProbability;
}


/* 
* Function to compute the Fresnel reflectance.
*/
double fresnelReflectance(double cosTheta, double refractiveIndex) {
    double r0 = (1.0 - refractiveIndex) / (1.0 + refractiveIndex);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

Vector3 RayTracer::traceRayPath(const Ray& ray, int depth) {
    if (depth >= maxDepth) {
        return Vector3(0, 0, 0);
    }

    HitRecord hitRecord;
    if (!scene->intersect(ray, hitRecord)) {
        return scene->backgroundColor;
    }

    Vector3 normal = hitRecord.normal;
    if (ray.direction.dot(normal) > 0) {
        normal = -normal;
    }

    // Get albedo (diffuse color or texture)
    Vector3 albedo = hitRecord.material.diffuseColor;
    if (hitRecord.material.hasTexture && hitRecord.getUV) {
        double u, v;
        hitRecord.getUV(hitRecord.point, u, v);
        albedo = hitRecord.material.getTextureColor(u, v);
    }

    // Russian Roulette termination
    if (depth > 3) {
        double maxReflectance = std::max(albedo.x, std::max(albedo.y, albedo.z));
        if (dist(rng) > maxReflectance) {
            return Vector3(0, 0, 0);
        }
        albedo = albedo / maxReflectance;
    }

    // Direct lighting calculation
    Vector3 directLight = estimateDirectLight(hitRecord, -ray.direction.normalize());
    Vector3 indirectLight(0, 0, 0);

    // Handle different material types
    if (hitRecord.material.isReflective) {
        
        Vector3 reflectedDir = reflect(ray.direction.normalize(), normal).normalize();
        Ray reflectedRay(hitRecord.point + normal * shadowBias, reflectedDir);
        
        Vector3 reflectedColor = traceRayPath(reflectedRay, depth + 1);
        indirectLight = reflectedColor * hitRecord.material.reflectivity;

    } else if (hitRecord.material.isRefractive) {
        normal = hitRecord.normal;
        double eta_i = 1.0;
        double eta_t = hitRecord.material.refractiveIndex;
        Vector3 incident = ray.direction.normalize();
        bool entering = incident.dot(normal) < 0;
        
        if (!entering) {
            std::swap(eta_i, eta_t);
            normal = -normal;
        }

        double fresnelCoeff = fresnel(incident, normal, eta_t, eta_i);
        Vector3 bias = normal * shadowBias;

        // Always calculate reflection
        Vector3 reflectDir = reflect(incident, normal).normalize();
        Ray reflectRay(hitRecord.point + bias, reflectDir);
        Vector3 reflectColor = traceRayPath(reflectRay, depth + 1);

        // Calculate refraction
        Vector3 refractDir = refract(incident, normal, eta_t, eta_i).normalize();
        Vector3 refractColor;

        if (refractDir.length() > 0.0) {
            Ray refractRay(hitRecord.point - bias, refractDir);
            refractColor = traceRayPath(refractRay, depth + 1);
            indirectLight = reflectColor * fresnelCoeff + refractColor * (1.0 - fresnelCoeff);
        } else {
            // Total internal reflection
            indirectLight = reflectColor;
        }

    } 
    else {
        // Diffuse material
        Vector3 newDir = randomInHemisphere(normal);
        double cosTheta = std::max(0.0, newDir.dot(normal));
        Ray newRay(hitRecord.point + normal * shadowBias, newDir);
        
        indirectLight = traceRayPath(newRay, depth + 1) * (albedo / M_PI) * cosTheta;
    }

    return directLight + indirectLight;
}


Vector3 RayTracer::estimateDirectLight(const HitRecord& hitRecord, const Vector3& viewDir) {
    Vector3 directLight(0, 0, 0);

    for (const auto& light : scene->lights) {
        if (light->type == Light::POINT) {
            // Handle point light
            auto pointLight = std::static_pointer_cast<PointLight>(light);
            Vector3 lightDir = (pointLight->getPosition() - hitRecord.point).normalize();
            double distance = (pointLight->getPosition() - hitRecord.point).length();

            // Shadow check
            Ray shadowRay(hitRecord.point + hitRecord.normal * shadowBias, lightDir);
            HitRecord shadowHit;
            if (scene->intersect(shadowRay, shadowHit) && shadowHit.t < distance) {
                continue; // In shadow
            }

            // Compute BRDF components
            double ndotl = std::max(0.0, hitRecord.normal.dot(lightDir));


            Vector3 diffuseBRDF = (hitRecord.material.diffuseColor * hitRecord.material.kd) / M_PI;
            if (hitRecord.material.hasTexture) {
                double u, v;
                hitRecord.getUV(hitRecord.point, u, v);
                diffuseBRDF = hitRecord.material.getTextureColor(u, v) * hitRecord.material.kd / M_PI;
            }

            // Specular component using Blinn-Phong model
            Vector3 halfVector = (lightDir + viewDir).normalize();
            double ndoth = std::max(0.0, hitRecord.normal.dot(halfVector));
            double specularFactor = pow(ndoth, hitRecord.material.specularExponent);
            Vector3 specularBRDF = hitRecord.material.specularColor * hitRecord.material.ks * ((hitRecord.material.specularExponent + 2.0) / (2.0 * M_PI)) * specularFactor;

            // Total BRDF
            Vector3 brdf = diffuseBRDF + specularBRDF;

            // Compute contribution
            Vector3 contribution = brdf * pointLight->intensity * ndotl;
            directLight += contribution;
        } else if (light->type == Light::AREA) {
            // Handle area light with multiple samples
            auto areaLight = std::static_pointer_cast<AreaLight>(light);
            Vector3 areaLightContribution(0, 0, 0);

            for (int i = 0; i < lightSamples; i++) {
                Vector3 lightDir;
                double distance, pdf;
                Vector3 intensity = areaLight->sample(hitRecord.point, lightDir, distance, pdf);

                // Shadow check
                Ray shadowRay(hitRecord.point + hitRecord.normal * shadowBias, lightDir);
                HitRecord shadowHit;
                if (scene->intersect(shadowRay, shadowHit) && shadowHit.t < distance) {
                    continue; // In shadow
                }

                // Compute BRDF components
                double ndotl = std::max(0.0, hitRecord.normal.dot(lightDir));
                double ndotl_light = std::max(0.0, areaLight->normal.dot(-lightDir));

                if (ndotl > 0 && ndotl_light > 0) {
                    // Diffuse component
                    Vector3 diffuseBRDF = (hitRecord.material.diffuseColor * hitRecord.material.kd) / M_PI;
                    if (hitRecord.material.hasTexture) {
                        double u, v;
                        hitRecord.getUV(hitRecord.point, u, v);
                        diffuseBRDF = hitRecord.material.getTextureColor(u, v) * hitRecord.material.kd / M_PI;
                    } 

                    // Specular component using Blinn-Phong model
                    Vector3 halfVector = (lightDir + viewDir).normalize();
                    double ndoth = std::max(0.0, hitRecord.normal.dot(halfVector));
                    double specularFactor = pow(ndoth, hitRecord.material.specularExponent);
                    Vector3 specularBRDF = hitRecord.material.specularColor * hitRecord.material.ks * ((hitRecord.material.specularExponent + 2.0) / (2.0 * M_PI)) * specularFactor;

                    // Total BRDF
                    Vector3 brdf = diffuseBRDF + specularBRDF;

                    // Compute contribution
                    Vector3 contribution = brdf * intensity * ndotl * ndotl_light / pdf;
                    areaLightContribution += contribution;
                }
            }

            // Average the contributions from all samples
            directLight += areaLightContribution / lightSamples;
        }
    }

    return directLight;
}

/*
* Function to compute Phong shading.
*/
Vector3 RayTracer::computeShadingPhong(const HitRecord& hitRecord, const Ray& ray, int depth) {
    // Ambient component
    double ambientIntensity = 0.25;

    Vector3 textureColor = hitRecord.material.diffuseColor;
    if (hitRecord.material.hasTexture) {
        double u, v;
        hitRecord.getUV(hitRecord.point, u, v);
        textureColor = hitRecord.material.getTextureColor(u, v);
    }

    Vector3 ambientColor = textureColor * ambientIntensity;

    // Initialize diffuse and specular components
    Vector3 diffuseColor(0, 0, 0);
    Vector3 specularColor(0, 0, 0);

    // View direction (from hit point to camera)
    Vector3 viewDir = -ray.direction.normalize();

    // Iterate over each light source
    for (const auto& light : scene->lights) {


        Vector3 lightDir = (light->getPosition() - hitRecord.point).normalize();
        Vector3 halfVector = (lightDir + viewDir).normalize();

        // Shadow check
        Ray shadowRay(hitRecord.point + hitRecord.normal * shadowBias, lightDir);
        HitRecord shadowHit;
        bool inShadow = false;
        if (scene->intersect(shadowRay, shadowHit)) {
            double lightDistance = (light->getPosition() - hitRecord.point).length();
            if (shadowHit.t < lightDistance) {
                inShadow = true;
            }
        }

        if (!inShadow) {
            // Diffuse shading (Lambertian)
            double diffuseFactor = std::max(0.0, hitRecord.normal.dot(lightDir));

            // Get texture color if available

            diffuseColor += textureColor * hitRecord.material.kd * diffuseFactor * light->intensity;

            // Specular shading (Blinn-Phong)
            double specularFactor = pow(std::max(0.0, hitRecord.normal.dot(halfVector)), hitRecord.material.specularExponent);
            specularColor += hitRecord.material.specularColor * hitRecord.material.ks * specularFactor * light->intensity;
        }
    }
    // Combine ambient, diffuse, and specular components
    Vector3 localColor = ambientColor + diffuseColor + specularColor;

    // Recursive reflection
    if (hitRecord.material.isReflective) {
        Vector3 normal = hitRecord.normal;
        if (ray.direction.dot(normal) > 0.0) {
            normal = -normal;
        }


        Vector3 reflectedDir = ray.direction - normal * 2 * ray.direction.dot(normal);
        Ray reflectedRay(hitRecord.point + normal * shadowBias, reflectedDir);
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

    // Read aperture and focus distance
    double aperture = cameraJson.value("aperture", 0.0);
    double focusDist = cameraJson.value("focusDistance", (cameraLookAt - cameraPosition).length());

    return Camera(cameraPosition, cameraLookAt, cameraUp, fov, aspectRatio, aperture, focusDist);
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
            auto light = std::make_shared<PointLight>(position, intensity);
            scene.addLight(light);
        } else if (lightType == "arealight") {
            Vector3 position(
                lightJson["position"][0],
                lightJson["position"][1],
                lightJson["position"][2]
            );
            Vector3 normal(
                lightJson["normal"][0],
                lightJson["normal"][1],
                lightJson["normal"][2]
            );
            Vector3 uVec(
                lightJson["u"][0],
                lightJson["u"][1],
                lightJson["u"][2]
            );
            Vector3 vVec(
                lightJson["v"][0],
                lightJson["v"][1],
                lightJson["v"][2]
            );

            double width = lightJson["width"];
            double height = lightJson["height"];
            Vector3 intensity(
                lightJson["intensity"][0],
                lightJson["intensity"][1],
                lightJson["intensity"][2]
            );
            auto light = std::make_shared<AreaLight>(position, normal, uVec, vVec, width, height, intensity);
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

            scene.addObject(sphere);
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

            scene.addObject(triangle);
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
            scene.addObject(cylinder);
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
    int specularExponent = materialJson.value("specularexponent", 1);

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

    bool isReflective = materialJson.value("isreflective", false);
    double reflectivity = materialJson.value("reflectivity", 0.0);

    bool isRefractive = materialJson.value("isrefractive", false);
    double refractiveIndex = materialJson.value("refractiveindex", 1.0);

    // Check for texture
    bool hasTexture = materialJson.contains("texturepath");
    std::string texturePath = hasTexture ? materialJson["texturepath"].get<std::string>() : "";

    Material material(ks, kd, specularExponent, isReflective, reflectivity, isRefractive,
                      refractiveIndex, diffuseColor, specularColor, hasTexture, texturePath);

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

void RayTracer::setToneMap(ToneMapping map) {
    toneMapping = map;
}

void RayTracer::setPixelSample(int n) {
    pixelSamples = n;
} 

void RayTracer::setLightSample(int n) {
    lightSamples = n;
}