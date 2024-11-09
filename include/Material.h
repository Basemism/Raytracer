// Material.h
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vector3.h"

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

    // Constructor
    Material();
    Material(double ks_, double kd_, int specularExponent_,
            bool isReflective_, double reflectivity_,
            bool isRefractive_, double refractiveIndex_,
            const Vector3& diffuseColor_, const Vector3& specularColor_)
        : ks(ks_), kd(kd_), specularExponent(specularExponent_),
          isReflective(isReflective_), reflectivity(reflectivity_),
          isRefractive(isRefractive_), refractiveIndex(refractiveIndex_), diffuseColor(diffuseColor_), specularColor(specularColor_) {}
};

#endif // MATERIAL_H
