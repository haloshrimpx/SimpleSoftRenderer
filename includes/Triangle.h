//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_TRIANGLE_H
#define SOFTRENDERER_TRIANGLE_H
#include <vector>

#include "Vector.h"
#include "Vertex.h"

namespace geom {
    class Mesh;

    /**
    * 三角形面结构体
    */
    class Triangle {
    public:
        int vertIndices[3]{};
        int normalIndices[3]{}; // 三角形顶点法线索引
        Color vertColors[3]{}; // 顶点颜色，计算后存储

        Triangle();

        Triangle(int va, int vb, int vc,
                 int nva, int nvb, int nvc,
                 const Color &ca, const Color &cb, const Color &cc);

        Triangle(const int vertIdxs[3], const int normals[3], const Color vertCols[3]);

        void getVertex(std::vector<Vertex> &vertices, const Mesh &mesh) const;

        void getVertNormals(std::vector<maths::Vector3> &normals, const Mesh &mesh) const;

        void getVertNormals(maths::Vector3 *triangleColors, const Mesh &mesh) const;

        void getVertColors(Color *triangleColors) const;

        void getVertColors(std::vector<Color> &triangleColors) const;

        void getVertex(Vertex *vertices, const Mesh &mesh) const;

        void getVertex(Vertex *vertices, const std::vector<Vertex> &verts) const;

        void print() const;
    };
}


#endif //SOFTRENDERER_TRIANGLE_H
