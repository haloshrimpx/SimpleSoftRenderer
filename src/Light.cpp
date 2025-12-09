//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Light.h"

#include "../includes/Matrix.h"
#include "../includes/shader_geometry.h"
#include "../includes/shader_transform.h"

Light::Light(const float intensity, const Color &color, const Transform &transform) {
    this->intensity = intensity;
    this->color = color;
    this->transform = transform;
}

DirectionalLight::DirectionalLight(const Color &lightColor, float intensity,
                                   const Transform &transform) : Light(intensity, lightColor, transform) {
}

LightType DirectionalLight::getType() const {
    return LightType::DIRECTIONAL;
}

maths::Vector3 DirectionalLight::getDirection() const {
    maths::Matrix4x4 rotationMatrix = shader::getRotationMatrix(transform.getRotation());

    // 获取z轴方向向量
    maths::Vector3 directionZ = {
        rotationMatrix.valueOf(0, 2),
        rotationMatrix.valueOf(1, 2),
        rotationMatrix.valueOf(2, 2),
    };
    // 归一化
    return directionZ.normalize();
}

PointLight::PointLight(const double range, const Color &lightColor, float intensity, const Transform &transform) : Light(
    intensity, lightColor, transform) {
    this->range = range;
}

LightType PointLight::getType() const {
    return LightType::POINT;
}

/**
 * 获取点光源衰减的光强度
 * @param pos 光源位置
 */
double PointLight::getAttenIntensity(const maths::Vector3 &pos) const {
    // x = distance / range
    const double dist = maths::Vector3::distance(pos, transform.getPosition());
    const double x = dist / range;
    // 光源衰减公式 1/(x^2)
    return 1 / (x * x);
}
