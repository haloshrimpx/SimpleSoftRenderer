//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Color.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>

/**
 * 计算漫反射
 * @param baseColor 物体颜色
 * @param lightColor 光照颜色
 * @param normal 法线
 * @param lightDir 指向光源的方向
 * @return
 */
Color calcDiffuse(const Color &baseColor, const Color &lightColor,
                  const maths::Vector3 &normal, const maths::Vector3 &lightDir) {
    double NdotL = maths::Vector3::dot(normal.normalize(), lightDir.normalize());
    // diffuse = lightCol * baseCol * max(0, NdotL)
    return (lightColor * baseColor) * (std::max(0.0, NdotL));
}

/**
 * 计算高光反射
 * @param baseColor 物体颜色
 * @param lightColor 光照颜色
 * @param normal 法线
 * @param gloss 光泽度
 * @param lightDir 指向光源的方向
 * @param viewDir 视角方向(指向点)
 * @return
 */
Color calcSpecular(const Color &baseColor, const Color &lightColor, const maths::Vector3 &normal, const double gloss,
                   const maths::Vector3 &lightDir, const maths::Vector3 &viewDir) {
    maths::Vector3 half = (lightDir + viewDir) * (1 / (lightDir + viewDir).getMagnitude());
    maths::Vector3 n = normal.normalize();

    // specular = (lightCol * baseCol) * max(0, n dot half)^gloss
    return (lightColor * baseColor) * std::pow(std::max(0.0, maths::Vector3::dot(n, half)), gloss);
}

Color::Color() {
    r = 0;
    g = 0;
    b = 0;
    a = 0;
}

Color::Color(const maths::Vector4 &vec) {
    r = static_cast<float>(vec.x);
    g = static_cast<float>(vec.y);
    b = static_cast<float>(vec.z);
    a = static_cast<float>(vec.w);
}

Color::Color(const float r, const float g, const float b, const float a) {
    this->r = r;
    this->g = g;
    this->b = g;
    this->a = b;
}

Color::Color(const Color &color) {
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;
}

Color::~Color() = default;


maths::Vector4 Color::toVector4() const {
    return {static_cast<double>(r), static_cast<double>(g), static_cast<double>(b), static_cast<double>(a)};
}

float Color::getBrightness() const {
    // 亮度值 = 0.299 * R + 0.587 * G + 0.114 * B
    return 0.299f * r + 0.587f * g + 0.114f * b;
}

Color Color::operator*(const Color &col) const {
    Color result;
    result.r = r * col.r;
    result.g = g * col.g;
    result.b = b * col.b;
    result.a = a * col.a;
    return {r, g, b, a};
}

Color Color::operator*(float value) const {
    return {r * value, g * value, b * value, a * value};
}

Color Color::operator*(double value) const {
    return {
        static_cast<float>(r * value),
        static_cast<float>(g * value),
        static_cast<float>(b * value),
        static_cast<float>(a * value)
    };
}

Color Color::operator+(const Color &color) const {
    return {r + color.r, g + color.g, b + color.b, a + color.a};
}

/**
 * 将颜色值从0-255映射到0-1
 * !!!!默认颜色分量值在0-255之间
 */
Color Color::toLinearColor() const {
    Color res;
    res.r = mapSRGBToLinear(r);
    res.g = mapSRGBToLinear(g);
    res.b = mapSRGBToLinear(b);
    res.a = a;
    return res;
}

/**
 * 将颜色值从0-1映射到0-255
 * !!!!默认颜色分量值在0-1之间
 */
Color Color::toSRGBColor() const {
    Color res;
    res.r = r * 255;
    res.g = g * 255;
    res.b = b * 255;
    res.a = a;
    return res;
}

/**
 * 将所有值限制在[0,255]
 * @return
 */
Color Color::sRGBClamp() const {
    Color res;
    res.r = std::clamp(r, 0.0f, 255.0f);
    res.g = std::clamp(g, 0.0f, 255.0f);
    res.b = std::clamp(b, 0.0f, 255.0f);
    res.a = a;
    return res;
}

/**
 * 将所有值限制在[0,1]
 * @return
 */
Color Color::linearClamp() const {
    Color res;
    res.r = std::clamp(r, 0.0f, 1.0f);
    res.g = std::clamp(g, 0.0f, 1.0f);
    res.b = std::clamp(b, 0.0f, 1.0f);
    res.a = a;
    return res;
}

Color Color::lerp(const Color &a, const Color &b, const double t) {
    return {
        static_cast<float>(std::lerp(a.r, b.r, t)),
        static_cast<float>(std::lerp(a.g, b.g, t)),
        static_cast<float>(std::lerp(a.b, b.b, t)),
        static_cast<float>(std::lerp(a.a, b.a, t)),
    };
}

Color Color::mapLinearToSRGBColor(const float r, const float g, const float b) {
    float red = (r / 2 + 0.5f) * 255;
    float green = (g / 2 + 0.5f) * 255;
    float blue = (b / 2 + 0.5f) * 255;

    return {red, green, blue, 1};
}

int Color::mapLinearToSRGB(const float valLinear) {
    float clamped = std::clamp(valLinear, 0.0f, 1.0f);
    return static_cast<int>(clamped * 255.0f);
}

float Color::mapSRGBToLinear(const int valSRGB) {
    return static_cast<float>(valSRGB) / 255.0f;
}

void Color::print() const {
    std::clog << "Color(" << r << ", " << g << ", " << b << ", " << a << ")" << std::endl;
}
