//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Triangle.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include "../includes/Mesh.h"
#include "../includes/Vertex.h"

namespace geometry {
    Triangle::Triangle() : vertIndices{} {
    }

    Triangle::Triangle(const int a, const int b, const int c) : vertIndices{a, b, c} {
        if (a < 0 || b < 0 || c < 0) {
            throw std::invalid_argument("Triangle indices cannot be negative");
        }
    }

    Triangle::Triangle(int a, int b, int c, const maths::Vector4 &normal) : vertIndices{a, b, c} {
        if (a < 0 || b < 0 || c < 0) {
            throw std::invalid_argument("Triangle indices cannot be negative");
        }
        faceNormal = normal;
    }

    Triangle::Triangle(const int indices[3]) : vertIndices{} {
        for (int i = 0; i < 3; ++i) {
            vertIndices[i] = indices[i];
        }
    }

    Triangle::Triangle(const int indices[3], const maths::Vector4 &normal) : vertIndices{} {
        for (int i = 0; i < 3; ++i) {
            vertIndices[i] = indices[i];
        }
        faceNormal = normal;
    }

    void Triangle::getVertex(std::vector<geometry::Vertex> &vertices, const Mesh &mesh) const {
        for (const int vertIndex: vertIndices) {
            vertices.push_back(mesh.vertices[vertIndex]);
        }
    }

    void Triangle::getVertex(geometry::Vertex *vertices, const Mesh &mesh) const {
        for (int i = 0; i < 3; ++i) {
            vertices[i] = mesh.vertices[vertIndices[i]];
        }
    }

    void Triangle::getVertex(geometry::Vertex *vertices, const std::vector<geometry::Vertex> &verts) const {
        for (int i = 0; i < 3; ++i) {
            vertices[i] = verts[vertIndices[i]];
        }
    }

    void Triangle::print() const {
        std::clog << "triangle: " << "\t" << vertIndices[0] << "\t" << vertIndices[1] << "\t" << vertIndices[2] <<
                std::endl;
        std::clog << "normal: ";
        faceNormal.print();
    }
}
