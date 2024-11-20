// Material.h
#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vector3.h"
#include <vector>
#include <memory>

class Material {
public:
    double ks;
    double kd;
    int specularExponent;
    bool isReflective;
    double reflectivity;
    bool isRefractive;
    double refractiveIndex;
    Vector3 diffuseColor;
    Vector3 specularColor;
    
    bool hasTexture;
    std::string texturePath;
    int textureWidth;
    int textureHeight;
    std::shared_ptr<std::vector<Vector3>> textureData;

    // Constructor
    Material();
    Material(double ks_, double kd_, int specularExponent_,
             bool isReflective_, double reflectivity_,
             bool isRefractive_, double refractiveIndex_,
             const Vector3& diffuseColor_, const Vector3& specularColor_,
             bool hasTexture_ = false, const std::string& texturePath_ = "")
        : ks(ks_), kd(kd_), specularExponent(specularExponent_),
          isReflective(isReflective_), reflectivity(reflectivity_),
          isRefractive(isRefractive_), refractiveIndex(refractiveIndex_),
          diffuseColor(diffuseColor_), specularColor(specularColor_),
          hasTexture(hasTexture_), texturePath(texturePath_),
          textureWidth(0), textureHeight(0) {
            if (hasTexture) {
              std::cout << "Loading texture from file: " << texturePath << std::endl;
              loadTexture();
              std::cout << "Texture loaded successfully." << std::endl;

            }
          }

    // Load texture from file
    void loadTexture();
    Vector3 getTextureColor(double u, double v) const;
};

#endif // MATERIAL_H
