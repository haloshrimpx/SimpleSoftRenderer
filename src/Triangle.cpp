//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Triangle.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include "../includes/Mesh.h"
#include "../includes/Vertex.h"

namespace geom {
    Triangle::Triangle() = default;

    Triangle::Triangle(const int va, const int vb, const int vc,
                       const int nva, const int nvb, const int nvc,
                       const Color &ca, const Color &cb, const Color &cc) : vertIndices{va, vb, vc},
                                                                            normalIndices{nva, nvb, nvc},
                                                                            vertColors{ca, cb, cc} {
        if (va < 0 || vb < 0 || vc < 0 || nva < 0 || nvb < 0 || nvc < 0) {
            throw std::invalid_argument("Triangle indices cannot be negative");
        }
    }


    Triangle::Triangle(const int vertIdxs[3], const int normals[3], const Color vertCols[3]) {
        for (int i = 0; i < 3; ++i) {
            vertIndices[i] = vertIdxs[i];
            normalIndices[i] = normals[i];
            vertColors[i] = vertCols[i];
        }
    }


    void Triangle::getVertex(std::vector<Vertex> &vertices, const Mesh &mesh) const {
        for (const int vertIndex: vertIndices) {
            vertices.push_back(mesh.vertices[vertIndex]);
        }
    }

    void Triangle::getVertNormals(std::vector<maths::Vector3> &normals, const Mesh &mesh) const {
        for (const int normal_index: normalIndices) {
            normals.push_back(mesh.normals[normal_index]);
        }
    }

    void Triangle::getVertNormals(maths::Vector3 *triangleColors, const Mesh &mesh) const {
        for (int i = 0; i < 3; ++i) {
            triangleColors[i] = mesh.normals[normalIndices[i]];
        }
    }

    void Triangle::getVertColors(std::vector<Color> &triangleColors) const {
        for (auto &vertColor: this->vertColors) {
            std::clog << "get vert color: " << vertColor.r << "\t" << vertColor.g << "\t" << vertColor.b << std::endl;
            triangleColors.push_back(vertColor);
        }
    }

    void Triangle::getVertColors(Color *triangleColors) const {
        for (int i = 0; i < 3; ++i) {
            triangleColors[i] = this->vertColors[i];
        }
    }

    void Triangle::getVertex(Vertex *vertices, const Mesh &mesh) const {
        for (int i = 0; i < 3; ++i) {
            vertices[i] = mesh.vertices[vertIndices[i]];
        }
    }

    void Triangle::getVertex(Vertex *vertices, const std::vector<Vertex> &verts) const {
        for (int i = 0; i < 3; ++i) {
            vertices[i] = verts[vertIndices[i]];
        }
    }

    void Triangle::print() const {
        std::clog << "\ttriangle: " << vertIndices[0] << "\t" << vertIndices[1] << "\t" << vertIndices[2] <<
                std::endl;
        std::clog << "\t normal: " << normalIndices[0] << "\t" << normalIndices[1] << "\t" << normalIndices[2] <<
                std::endl;

        std::clog << "\t color0: " << vertColors[0].r << "\t" << vertColors[0].g << "\t" << vertColors[0].b << std::endl;
        std::clog << "\t color1: " << vertColors[1].r << "\t" << vertColors[1].g << "\t" << vertColors[1].b << std::endl;
        std::clog << "\t color2: " << vertColors[2].r << "\t" << vertColors[2].g << "\t" << vertColors[2].b << std::endl;
    }
}
