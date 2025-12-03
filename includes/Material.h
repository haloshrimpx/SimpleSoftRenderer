//
// Created by Haloshrimp on 2025/12/3.
//

#ifndef SOFTRENDERER_MATERIAL_H
#define SOFTRENDERER_MATERIAL_H
#include "Color.h"

class Material {
public:
    Color baseColor;
    Color emissiveColor;
    float gloss;

    Material();

    Material(const Color &base, const Color &emissive, float shininess);

    ~Material() = default;
};

#endif //SOFTRENDERER_MATERIAL_H
