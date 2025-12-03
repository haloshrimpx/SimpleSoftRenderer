//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_OBJECT_H
#define SOFTRENDERER_OBJECT_H
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"

class Object {
public:
    geometry::Mesh mesh;
    Material material;
    Transform transform;

    Object(const std::vector<std::array<maths::Vector3, 3> > &triangles, const Transform &transform,
           const Material &material);

    Object(const std::vector<std::array<geometry::Vertex, 3> > &vertices, const Transform &transform,
           const Material &material);

    Object(const geometry::Mesh &mesh, const Transform &transform, const Material &material);

    ~Object();
};

#endif //SOFTRENDERER_OBJECT_H
