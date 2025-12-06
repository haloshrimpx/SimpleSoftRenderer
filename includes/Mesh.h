//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_MESH_H
#define SOFTRENDERER_MESH_H
#include <map>
#include <string>
#include <vector>
#include <array>

#include "Triangle.h"
#include "Vertex.h"


namespace maths {
    class Vector3;
}

namespace geom {
    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<maths::Vector3> normals;
        std::vector<Triangle> triangles;

        Mesh() = default;

        Mesh(const Mesh &mesh);

        // explicit Mesh(const std::vector<std::array<maths::Vector3, 3> > &triangleVerts);
        //
        // explicit Mesh(const std::vector<std::array<Vertex, 3> > &triangleVerts);

        /**
         * 构造物体网格
         * 通过读取解析器生成网格时，默认是可信的，不会做任何检查
         * @param vertices
         * @param triangles
         * @param normals
         */
        Mesh(const std::vector<Vertex> &vertices, const std::vector<Triangle> &triangles,
             const std::vector<maths::Vector3> &normals);

        int isVertexExists(const std::string &vertKey);

        int isNormalExists(const std::string &normalKey);

        void buildVertexList(const std::vector<Vertex> &vertInput);

        void buildNormalList(const std::vector<maths::Vector3> &normalInput);

        void reconstructVertMap();

        void reconstructNormalsMap();

        static int addNewVertex(const Vertex &vert, std::vector<Vertex> &newVertices,
                                std::map<std::string, int> &checker);

        static int addNewNormal(const maths::Vector3 &normal, std::vector<maths::Vector3> &newNormals,
                                std::map<std::string, int> &checker);

        void print() const;

    private:
        std::map<std::string, int> vertIndexMap;
        std::map<std::string, int> normalIndexMap;
        // void addTriangle(const Triangle &triangle);
        // void removeTriangle();
    };
}
#endif //SOFTRENDERER_MESH_H
