//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Mesh.h"
#include "../includes/Vector.h"
#include <iostream>
using namespace maths;

namespace geom {
    Mesh::Mesh(const Mesh &mesh) {
        this->vertices = mesh.vertices;
        this->normals = mesh.normals;
        this->triangles = mesh.triangles;
        this->vertIndexMap = mesh.vertIndexMap;
    }

    // Mesh::Mesh(const std::vector<std::array<Vector3, 3> > &triangleVerts) {
    //     std::vector<geom::Vertex> newVerts;
    //
    //     // 遍历输入，添加到临时数组中
    //     for (const auto &vertex: triangleVerts) {
    //         // Vertex verts[3];
    //         for (int i = 0; i < 3; ++i) {
    //             geom::Vertex newVert{vertex[i]};
    //             newVerts.push_back(newVert);
    //         }
    //     }
    //     // 整理顶点
    //     buildVertexList(newVerts);
    //
    //     for (const auto &triangle_vert: triangleVerts) {
    //         int indices[3] = {};
    //         int existCount = 0; // 检查三个顶点是否都存在
    //         for (int i = 0; i < 3; ++i) {
    //             geom::Vertex newVert{triangle_vert[i]};
    //             std::string key = newVert.formatVertex();
    //             int index = isVertexExists(key);
    //             if (index < 0) break;
    //             indices[i] = index;
    //             existCount++;
    //         }
    //
    //         if (existCount == 3) {
    //             triangles.emplace_back(indices);
    //             std::clog << "add a triangle!" << std::endl;
    //         }
    //     }
    //     std::clog << "initialize a mesh, vertex count = " << vertices.size() <<
    //             "\t triangles count = " << triangles.size() << std::endl;
    // }

    // Mesh::Mesh(const std::vector<std::array<geom::Vertex, 3> > &triangleVerts) {
    //     std::vector<geom::Vertex> newVerts;
    //
    //     // 遍历输入，添加到临时数组中
    //     for (const auto &vertex: triangleVerts) {
    //         for (int i = 0; i < 3; ++i) {
    //             const geom::Vertex &newVert = vertex[i];
    //             newVerts.push_back(newVert);
    //         }
    //     }
    //     // 整理顶点
    //     buildVertexList(newVerts);
    //
    //     for (const auto &triangle_vert: triangleVerts) {
    //         int indices[3] = {};
    //         int existCount = 0; // 检查三个顶点是否都存在
    //         for (int i = 0; i < 3; ++i) {
    //             const geom::Vertex &newVert = triangle_vert[i];
    //             std::string key = newVert.formatVertex();
    //             int index = isVertexExists(key);
    //             if (index < 0) break;
    //             indices[i] = index;
    //             existCount++;
    //         }
    //
    //         if (existCount == 3) {
    //             triangles.emplace_back(indices);
    //             std::clog << "add a triangle!" << std::endl;
    //         }
    //     }
    //     std::clog << "initialize a mesh, vertex count = " << vertices.size() <<
    //             "\t triangles count = " << triangles.size() << std::endl;
    // }

    Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<Triangle> &triangles,
               const std::vector<maths::Vector3> &normals) {
        this->vertices = vertices;
        this->triangles = triangles;
        this->normals = normals;

        reconstructNormalsMap();
        reconstructVertMap();
    }

    int Mesh::isVertexExists(const std::string &vertKey) {
        if (vertIndexMap.contains(vertKey)) {
            return vertIndexMap[vertKey];
        }
        return -1;
    }

    int Mesh::isNormalExists(const std::string &normalKey) {
        if (normalIndexMap.contains(normalKey)) {
            return normalIndexMap[normalKey];
        }
        return -1;
    }

    void Mesh::buildVertexList(const std::vector<geom::Vertex> &vertInput) {
        const size_t size = vertInput.size();
        for (int i = 0; i < size; ++i) {
            std::string strKey = vertInput[i].formatVertex();

            // 索引不存在
            if (isVertexExists(strKey) < 0) {
                size_t newIndex = vertices.size();
                vertices.push_back(vertInput[i]);
                vertIndexMap.insert(std::pair(strKey, newIndex));
                std::clog << "add vertex = " << strKey << "\t index = " << newIndex << std::endl;
            }
        }
    }

    void Mesh::buildNormalList(const std::vector<maths::Vector3> &normalInput) {
        const size_t size = normalInput.size();
        for (int i = 0; i < size; ++i) {
            std::string strKey = normalInput[i].formatVector3();

            // 索引不存在
            if (isVertexExists(strKey) < 0) {
                size_t newIndex = normals.size();
                normals.push_back(normalInput[i]);
                normalIndexMap.insert(std::pair(strKey, newIndex));
                std::clog << "add vertex = " << strKey << "\t index = " << newIndex << std::endl;
            }
        }
    }

    void Mesh::reconstructVertMap() {
        vertIndexMap.clear();
        const size_t size = vertices.size();
        for (int i = 0; i < size; ++i) {
            std::string strKey = vertices[i].formatVertex();
            // 判断顶点是否已经存在
            if (isVertexExists(strKey) < 0) {
                vertIndexMap.insert(std::pair(strKey, i));
            }
        }
    }

    void Mesh::reconstructNormalsMap() {
        normalIndexMap.clear();
        const size_t size = normals.size();
        for (int i = 0; i < size; ++i) {
            std::string strKey = normals[i].formatVector3();
            // 判断顶点是否已经存在
            if (isVertexExists(strKey) < 0) {
                normalIndexMap.insert(std::pair(strKey, i));
            }
        }
    }

    /**
     * 向一个临时顶点列表中添加顶点
     * @param vert
     * @param newVertices
     * @param checker
     * @return
     */
    int Mesh::addNewVertex(const geom::Vertex &vert, std::vector<geom::Vertex> &newVertices,
                           std::map<std::string, int> &checker) {
        std::string key = vert.formatVertex();
        // 寻找顶点是否已经存在
        if (checker.contains(key)) {
            return checker[key];
        }

        unsigned long long newIndex = newVertices.size();
        newVertices.push_back(vert);
        checker.insert(std::pair(key, newIndex));
        return newIndex;
    }

    int Mesh::addNewNormal(const maths::Vector3 &normal, std::vector<Vector3> &newNormals,
                           std::map<std::string, int> &checker) {
        std::string key = normal.formatVector3();
        // 寻找顶点是否已经存在
        if (checker.contains(key)) {
            return checker[key];
        }

        size_t newIndex = newNormals.size();
        newNormals.push_back(normal);
        checker.insert(std::pair(key, newIndex));

        return newIndex;
    }

    void Mesh::print() const {
        std::clog << "Mesh: " << std::endl;
        std::clog << "\t vertices: count " << vertices.size() << std::endl;
        for (int i = 0; i < vertices.size(); ++i) {
            std::clog << "\t vertex: " << i << "\t";
            vertices[i].pos.print();
        }

        std::clog << "\t triangles count: " << triangles.size() << std::endl;
        for (const auto &triangle: triangles) {
            std::clog << "\t";
            triangle.print();
        }

        std::clog << "\t normals: count " << normals.size() << std::endl;
        for (int i = 0; i < normals.size(); ++i) {
            std::clog << "\t";
            normals[i].print();
        }
    }
}
