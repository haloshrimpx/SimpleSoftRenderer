//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_LIGHT_H
#define SOFTRENDERER_LIGHT_H
#include "Color.h"
#include "Transform.h"

class Transform;
class Color;

enum class LightType {
    DIRECTIONAL,
    POINT,
};

class Light {
public:
    float intensity;
    Color color;
    Transform transform;

    Light(float intensity, const Color &color, const Transform &transform);

    virtual ~Light() = default;

    [[nodiscard]] virtual LightType getType() const = 0;
};

class DirectionalLight : public Light {
public:
    DirectionalLight(const Color &lightColor, float intensity, const Transform &transform);

    [[nodiscard]] LightType getType() const override;

    [[nodiscard]] maths::Vector3 getDirection() const;
};

class PointLight : public Light {
public:
    double range;

    PointLight(double range, const Color &lightColor, float intensity, const Transform &transform);

    [[nodiscard]] LightType getType() const override;

    [[nodiscard]] double getAttenIntensity(const maths::Vector3 &pos) const;
};

#endif //SOFTRENDERER_LIGHT_H
