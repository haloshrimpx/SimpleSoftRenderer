//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Object.h"
#include <array>
#include <vector>

class Transform;

namespace maths {
    class Vector3;
}

// Object::Object(const std::vector<std::array<maths::Vector3, 3> > &triangles, const Transform &transform,
//                const Material &material) {
//     this->mesh = geom::Mesh(triangles);
//     this->transform = transform;
//     this->material = material;
// }
//
// Object::Object(const std::vector<std::array<geom::Vertex, 3> > &vertices, const Transform &transform,
//                const Material &material) {
//     this->mesh = geom::Mesh(vertices);
//     this->transform = transform;
//     this->material = material;
// }

Object::Object(const geom::Mesh &mesh, const Transform &transform, const Material &material) {
    this->mesh = mesh;
    this->transform = transform;
    this->material = material;
}

Object::Object(const Object &other) {
    this->mesh = other.mesh;
    this->transform = other.transform;
    this->material = other.material;
}

Object::~Object() = default;
