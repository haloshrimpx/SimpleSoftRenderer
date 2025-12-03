//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_LIGHT_H
#define SOFTRENDERER_LIGHT_H
#include "Color.h"
#include "Transform.h"

class Transform;
class Color;

class Light {
public:
    float intensity;
    Color color;
    Transform transform;

    Light(float intensity, const Color &color, const Transform &transform);

    ~Light() = default;
};

class DirectionalLight : public Light {
public:
    DirectionalLight(const Color &lightColor, float intensity, const Transform &transform);

    maths::Vector4 getDirection() const;
};

#endif //SOFTRENDERER_LIGHT_H
