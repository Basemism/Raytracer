// PointLight.h
#pragma once
#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light {
public:
    Vector3 position;

    PointLight(const Vector3& position_, const Vector3& intensity_)
        : Light(LightType::POINT, intensity_), position(position_) {}

    virtual Vector3 getPosition() const override;
};

#endif // POINTLIGHT_H
