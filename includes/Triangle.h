//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_TRIANGLE_H
#define SOFTRENDERER_TRIANGLE_H
#include <vector>

#include "Vector.h"
#include "Vertex.h"

namespace geometry {
    class Mesh;

    /**
    * 三角形面结构体
    */
    class Triangle {
    public:
        int vertIndices[3];
        maths::Vector4 faceNormal;

        Triangle();

        Triangle(int a, int b, int c);

        Triangle(int a, int b, int c, const maths::Vector4 &normal);

        explicit Triangle(const int indices[3]);

        explicit Triangle(const int indices[3], const maths::Vector4 &normal);

        void getVertex(std::vector<Vertex> &vertices, const geometry::Mesh &mesh) const;

        void getVertex(Vertex *vertices, const Mesh &mesh) const;

        void getVertex(Vertex *vertices, const std::vector<Vertex> &verts) const;

        void print() const;
    };
}


#endif //SOFTRENDERER_TRIANGLE_H
