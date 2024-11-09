// Material.h
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vector3.h"

class Material {
public:
    double ks;
    double kd;
    double specularExponent;
    Vector3 diffuseColor;
    Vector3 specularColor;
    bool isReflective;
    double reflectivity;
    bool isRefractive;
    double refractiveIndex;

    Material();
    Material(double ks, double kd, double specularExponent,
             const Vector3& diffuseColor, const Vector3& specularColor,
             bool isReflective, double reflectivity,
             bool isRefractive, double refractiveIndex)
        : ks(ks), kd(kd), specularExponent(specularExponent),
          diffuseColor(diffuseColor), specularColor(specularColor),
          isReflective(isReflective), reflectivity(reflectivity),
          isRefractive(isRefractive), refractiveIndex(refractiveIndex) {}
};


#endif // MATERIAL_H
