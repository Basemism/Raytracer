// AreaLight.h
#pragma once
#ifndef AREALIGHT_H
#define AREALIGHT_H

#include "Light.h"
#include <random>

class AreaLight : public Light {
public:
    Vector3 position;
    Vector3 normal;
    Vector3 uVec; // Local x-axis
    Vector3 vVec; // Local y-axis
    double width;
    double height;

    AreaLight(const Vector3& position_, const Vector3& normal_,
              const Vector3& uVec_, const Vector3& vVec_,
              double width_, double height_, const Vector3& intensity_)
        : Light(LightType::AREA, intensity_), position(position_), normal(normal_.normalize()),
          uVec(uVec_.normalize()), vVec(vVec_.normalize()), width(width_), height(height_) {}

    // Sample a point on the light's surface
    virtual Vector3 sample(const Vector3& point, Vector3& lightDir, double& distance, double& pdf) const override;
};

#endif // AREALIGHT_H
