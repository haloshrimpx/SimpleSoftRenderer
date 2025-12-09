//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Object.h"
#include <array>
#include <vector>

#include "../includes/obj_reader.h"

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

Object::Object(const std::string &name, const geom::Mesh &mesh, const Transform &transform, const Material &material) {
    this->name = name;
    this->mesh = mesh;
    this->transform = transform;
    this->material = material;
}

Object::Object(const std::string &name, const std::string &filepath, const Transform &transform, const Material &material) {
    this->name = name;
    objreader::tryReadObjFile(filepath, this->mesh);
    this->transform = transform;
    this->material = material;
}

Object::Object(const Object &other) {
    this->name = other.name;
    this->mesh = other.mesh;
    this->transform = other.transform;
    this->material = other.material;
}

Object::~Object() = default;
