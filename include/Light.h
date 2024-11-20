// Light.h
#ifndef LIGHT_H
#define LIGHT_H

#include "Vector3.h"
#include "Ray.h"

class Light {
public:
    enum LightType {
        POINT,
        AREA
    };

    LightType type;
    Vector3 intensity;

    Light(LightType type_, const Vector3& intensity_) : type(type_), intensity(intensity_) {}
    virtual ~Light() {}

    // For area lights
    virtual Vector3 sample(const Vector3& point, Vector3& lightDir, double& distance, double& pdf) const;

    // For point lights
    virtual Vector3 getPosition() const;
};

#endif // LIGHT_H
