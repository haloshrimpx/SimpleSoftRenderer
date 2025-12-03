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

namespace geometry {
    class Mesh {
    public:
        std::vector<geometry::Vertex> vertices;
        std::vector<geometry::Triangle> triangles;

        Mesh() = default;

        Mesh(const Mesh &mesh);

        explicit Mesh(const std::vector<std::array<maths::Vector3, 3> > &triangleVerts);

        explicit Mesh(const std::vector<std::array<geometry::Vertex, 3> > &triangleVerts);

        int isVertexExists(const geometry::Vertex &vert);

        int isVertexExists(const std::string &vertKey);

        void buildVertexList(const std::vector<geometry::Vertex> &vertInput);

        void reconstructVertMap();

        static int addNewVertex(const geometry::Vertex &vert, std::vector<geometry::Vertex> &newVertices,
                                std::map<std::string, int> &checker);

        void print() const;

    private:
        std::map<std::string, int> vertIndexMap;
        // void addTriangle(const Triangle &triangle);
        // void removeTriangle();
    };

}
#endif //SOFTRENDERER_MESH_H
