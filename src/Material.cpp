// Material.cpp
#include "Material.h"

Material::Material()
    : ks(0.0), kd(0.0), specularExponent(0.0),
      diffuseColor(0.0, 0.0, 0.0), specularColor(0.0, 0.0, 0.0),
      isReflective(false), reflectivity(0.0),
      isRefractive(false), refractiveIndex(1.0) {}

Material::Material(double ks, double kd, double specularExponent,
                   const Vector3& diffuseColor, const Vector3& specularColor,
                   bool isReflective, double reflectivity,
                   bool isRefractive, double refractiveIndex)
    : ks(ks), kd(kd), specularExponent(specularExponent),
      diffuseColor(diffuseColor), specularColor(specularColor),
      isReflective(isReflective), reflectivity(reflectivity),
      isRefractive(isRefractive), refractiveIndex(refractiveIndex) {}
