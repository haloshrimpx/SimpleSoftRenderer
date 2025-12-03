//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Light.h"

Light::Light(const float intensity, const Color &color, const Transform &transform) {
    this->intensity = intensity;
    this->color = color;
    this->transform = transform;
}

DirectionalLight::DirectionalLight(const Color &lightColor, float intensity,
                                   const Transform &transform) : Light(intensity, lightColor, transform) {
}

maths::Vector4 DirectionalLight::getDirection() const {
    return maths::VECTOR4_ZERO; //TODO
}
