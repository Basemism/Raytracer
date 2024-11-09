// Material.cpp
#include "Material.h"

Material::Material()
    : ks(0.0), kd(0.0), specularExponent(0.0),
      isReflective(false), reflectivity(0.0),
      isRefractive(false), refractiveIndex(1.0),
      diffuseColor(0.0, 0.0, 0.0), specularColor(0.0, 0.0, 0.0) {}
