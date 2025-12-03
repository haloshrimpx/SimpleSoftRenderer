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

Object::Object(const std::vector<std::array<maths::Vector3, 3> > &triangles,
               const Transform &transform) {
    this->mesh = geometry::Mesh(triangles);
    this->transform = transform;
}

Object::Object(const std::vector<std::array<geometry::Vertex, 3> > &vertices, const Transform &transform) {
    this->mesh = geometry::Mesh(vertices);
    this->transform = transform;
}

Object::Object(const geometry::Mesh &mesh, const Transform &transform) {
    this->mesh = mesh;
    this->transform = transform;
}

Object::~Object() = default;