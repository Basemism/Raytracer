// Material.h
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vector3.h"

class Material {
public:
    double ks;                   // Specular coefficient
    double kd;                   // Diffuse coefficient
    double specularExponent;     // Specular exponent (shininess)
    Vector3 diffuseColor;        // Diffuse color
    Vector3 specularColor;       // Specular color
    bool isReflective;           // Is the material reflective?
    double reflectivity;         // Reflectivity coefficient
    bool isRefractive;           // Is the material refractive?
    double refractiveIndex;      // Refractive index

    // Constructors
    Material();
    Material(double ks, double kd, double specularExponent,
             const Vector3& diffuseColor, const Vector3& specularColor,
             bool isReflective, double reflectivity,
             bool isRefractive, double refractiveIndex);
};

#endif // MATERIAL_H
