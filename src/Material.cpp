// Material.cpp
#include "Material.h"

// Default constructor initializes material properties to zero
Material::Material()
    : ambient(0.0, 0.0, 0.0),
      diffuse(0.0, 0.0, 0.0),
      specular(0.0, 0.0, 0.0),
      shininess(0.0),
      reflectivity(0.0) {}

// Parameterized constructor
Material::Material(const Vector3& ambient, const Vector3& diffuse, const Vector3& specular, double shininess, double reflectivity)
    : ambient(ambient),
      diffuse(diffuse),
      specular(specular),
      shininess(shininess),
      reflectivity(reflectivity) {}
