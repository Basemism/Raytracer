// Light.h
#ifndef LIGHT_H
#define LIGHT_H

#include "Vector3.h"

/**
 * @brief A class representing a point light source.
 */
class Light {
public:
    Vector3 position;
    Vector3 intensity;

    // Constructor
    Light(const Vector3& position, const Vector3& intensity);
};

#endif // LIGHT_H
