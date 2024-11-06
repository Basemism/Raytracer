// Material.h
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vector3.h"

/**
 * @brief A class representing the material properties of an object.
 */
class Material {
public:
    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;

    double shininess;
    double reflectivity;

    Material();

    Material(const Vector3& ambient, const Vector3& diffuse, const Vector3& specular, double shininess, double reflectivity = 0.0);
};

#endif // MATERIAL_H
