// Light.cpp
#include "Light.h"
#include <random>

Vector3 Light::sample(const Vector3& point, Vector3& lightDir, double& distance, double& pdf) const {
    return Vector3(0, 0, 0);
}

Vector3 Light::getPosition() const {
    return Vector3(0, 0, 0);
}