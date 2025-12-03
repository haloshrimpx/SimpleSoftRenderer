//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_COLOR_H
#define SOFTRENDERER_COLOR_H
#include "Vector.h"
class Color;
class Material;


Color calcDiffuse(const Color &baseColor, const Color &lightColor, const maths::Vector3 &normal,
                  const maths::Vector3 &lightDir);

/**
 * 颜色类
 * x - r, y - g, z - b, w - a
 */
class Color {
public:
    float r; // 0-1
    float g; // 0-1
    float b; // 0-1
    float a; // 0-1

    Color();

    explicit Color(const maths::Vector4 &vec);

    Color(float r, float g, float b, float a = 1);

    ~Color();

    maths::Vector4 toVector4() const;

    float getBrightness() const;

    Color operator*(const Color &col) const;

    Color operator*(float value) const;

    Color operator*(double value) const;

    Color operator+(const Color &color) const;

    Color toLinearColor() const;

    Color toSRGBColor() const;

    static Color lerp(const Color &a, const Color &b, double t);

    static Color mapLinearToSRGBColor(float r, float g, float b);

    static int mapLinearToSRGB(float valLinear);

    static float mapSRGBToLinear(int valSRGB);

    void print() const;
};

const Color COLOR_INVALID(-1, -1, -1, -1);
const Color COLOR_WHITE(1, 1, 1, 1);
const Color COLOR_BLACK(0, 0, 0, 1);
const Color COLOR_RED(1, 0, 0, 1);
const Color COLOR_GREEN(0, 1, 0, 1);
const Color COLOR_BLUE(0, 0, 1, 1);
const Color COLOR_YELLOW(1, 1, 0, 1);
const Color COLOR_MAGENTA(1, 0, 1, 1);
const Color COLOR_CYAN(0, 1, 1, 1);
#endif //SOFTRENDERER_COLOR_H
