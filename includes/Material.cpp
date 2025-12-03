//
// Created by Haloshrimp on 2025/12/3.
//

#include "Material.h"

Material::Material() {
    baseColor = COLOR_WHITE;
    emissiveColor = COLOR_BLACK;
    gloss = 0;
}

Material::Material(const Color &base, const Color &emissive, float shininess) {
    baseColor = base;
    emissiveColor = emissive;
    this->gloss = shininess;
}
