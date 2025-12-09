//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/shader_geometry.h"

#include <cmath>
#include <iostream>
#include <ostream>

namespace shader {
    bool isInsideTriangle(const geom::Vertex vert[3], const maths::Vector2 &position) {
        const maths::Vector2 &a = vert[0].pos.toVector2();
        const maths::Vector2 &b = vert[1].pos.toVector2();
        const maths::Vector2 &c = vert[2].pos.toVector2();

        // 三点叉乘求p的位置
        double d1 = threeDotCross(a, b, position);
        double d2 = threeDotCross(b, c, position);
        double d3 = threeDotCross(c, a, position);

        // p在内部或边界上
        if ((d1 <= 0 && d2 <= 0 && d3 <= 0) || (d1 >= 0 && d2 >= 0 && d3 >= 0)) {
            return true;
        }

        return false;
    }

    geom::Rectangle calcTriangleBound(const geom::Vertex vert[3]) {
        // 给定三个顶点 a b c
        // x min = min(ax, bx, cx)
        // y min = min(ay, by, cy)

        maths::Vector3 v0 = vert[0].pos.toVector3();
        maths::Vector3 v1 = vert[1].pos.toVector3();
        maths::Vector3 v2 = vert[2].pos.toVector3();

        // 计算最大最小边界点
        double dminX = std::min(v0.x, std::min(v1.x, v2.x));
        double dmaxX = std::max(v0.x, std::max(v1.x, v2.x));
        double dminY = std::min(v0.y, std::min(v1.y, v2.y));
        double dmaxY = std::max(v0.y, std::max(v1.y, v2.y));

        // 取整，避免三角形出现裂缝
        // 左、下边界向下取整，右、上边界向上取整
        int minX = static_cast<int>(std::floor(dminX - 0.5));
        int maxX = static_cast<int>(std::ceil(dmaxX + 0.5));
        int minY = static_cast<int>(std::floor(dminY - 0.5));
        int maxY = static_cast<int>(std::ceil(dmaxY + 0.5));

        return {minX, minY, maxX - minX, maxY - minY,};
    }

    bool clipVertices(const maths::Vector4 &plane,
                      std::vector<geom::Vertex> &vertices, std::vector<maths::Vector3> &normals,
                      std::vector<Color> &colors) {
        // // 检查w分量是否为0
        // for (const geom::Vertex &vert: vertices) {
        //     if (vert.pos.w == 0.0) {
        //         std::clog << "w of vertex is 0, can not clip" << std::endl;
        //         return false;
        //     }
        // }

        // 空检查
        if (vertices.empty()) {
            return false;
        }

        // 输入的三角形顶点和顶点法线数量应该是相等的
        if (vertices.size() != normals.size()) {
            std::clog << "vertices input not equal to normals input!!" << std::endl;
            return false;
        }

        std::vector<geom::Vertex> vertOutput;
        std::vector<maths::Vector3> normalsOutput;
        std::vector<Color> colorsOutput;

        size_t vertCount = vertices.size();
        std::vector<double> dotProducts(vertCount);

        // 预计算所有顶点与平面的点积
        for (size_t i = 0; i < vertCount; ++i) {
            dotProducts[i] = calcDotOfPlane(plane, vertices[i].pos);
        }

        for (size_t i = 0; i < vertCount; ++i) {
            // 同时插值顶点、法线、颜色
            size_t prevIndex = (i - 1 + vertCount) % vertCount;

            const geom::Vertex &currentVert = vertices[i];
            const geom::Vertex &prevVert = vertices[prevIndex];

            const maths::Vector3 &currentNormal = normals[i];
            const maths::Vector3 &prevNormal = normals[prevIndex];

            const Color &currentColor = colors[i];
            const Color &prevColor = colors[prevIndex];

            double dCurrent = dotProducts[i];
            double dPrev = dotProducts[prevIndex]; // 取模运算，循环遍历

            // 使用一个小的阈值来处理浮点数比较
            constexpr double epsilon = 1e-6;

            // 当前点在平面内
            if (dCurrent >= -epsilon) {
                if (dPrev < -epsilon) {
                    // 有一个点在平面外，插值计算交点
                    double t = dPrev / (dPrev - dCurrent);

                    geom::Vertex vertexIsc = geom::Vertex::lerp(prevVert, currentVert, t);
                    maths::Vector3 normalIsc = maths::Vector3::lerp(currentNormal, prevNormal, t);
                    Color colorIsc = Color::lerp(currentColor, prevColor, t);

                    vertOutput.push_back(vertexIsc);
                    normalsOutput.push_back(normalIsc);
                    colorsOutput.push_back(colorIsc);
                }
                // 添加当前点

                vertOutput.push_back(currentVert);
                normalsOutput.push_back(currentNormal);
                colorsOutput.push_back(currentColor);
            }
            // 前一个点在内部，计算插值点
            else if (dPrev >= -epsilon) {
                double t = dPrev / (dPrev - dCurrent);

                geom::Vertex vertexIsc = geom::Vertex::lerp(prevVert, currentVert, t);
                maths::Vector3 normalIsc = maths::Vector3::lerp(currentNormal, prevNormal, t);
                Color colorIsc = Color::lerp(currentColor, prevColor, t);

                vertOutput.push_back(vertexIsc);
                normalsOutput.push_back(normalIsc);
                colorsOutput.push_back(colorIsc);
            }
        }

        vertices = vertOutput;
        normals = normalsOutput;
        colors = colorsOutput;

        return true;
    }

    double calcDotOfPlane(const maths::Vector4 &plane, const maths::Vector4 &vec) {
        return maths::Vector4::dot(plane, vec);
    }

    bool isTriangleFacing(const std::vector<geom::Vertex> &vertices) {
        // 三角形为逆时针绕序

        // 先将顶点转换到NDC
        maths::Vector4 ndcVecs[3];
        for (int i = 0; i < 3; ++i) {
            maths::Vector4 ndcVec = vertices[3].pos;
            ndcVec.x = ndcVec.x / ndcVec.w;
            ndcVec.y = ndcVec.y / ndcVec.w;
            ndcVec.z = ndcVec.z / ndcVec.w;
            ndcVecs[i] = ndcVec;
        }

        // 计算法线（v0->v1 × v0->v2）
        maths::Vector4 edge1 = ndcVecs[1] - ndcVecs[0];
        maths::Vector4 edge2 = ndcVecs[2] - ndcVecs[0];
        maths::Vector4 normal = maths::Vector4::cross(edge1, edge2);

        // 对于逆时针三角形，在标准视图下：
        // normal.z > 0: 正面朝向观察者
        // normal.z < 0: 背面朝向观察者
        return normal.z > 0.0;
    }

    void clipMesh(geom::Mesh &mesh) {
        const maths::Vector4 planes[7] = {
            // PLANE_W,
            PLANE_NEAR,
            PLANE_FAR,
            PLANE_LEFT,
            PLANE_RIGHT,
            PLANE_TOP,
            PLANE_BOTTOM,
        };

        std::vector<geom::Triangle> &trianglesInput = mesh.triangles;
        std::vector<geom::Vertex> &verticesInput = mesh.vertices;
        std::vector<maths::Vector3> &normalsInput = mesh.normals;

        std::vector<geom::Triangle> trianglesOutput;
        std::vector<geom::Vertex> verticesOutput;
        std::vector<maths::Vector3> normalsOutput;

        std::map<std::string, int> vertChecker; // 三角索引网格的检查
        std::map<std::string, int> normalChecker; // 检查法线


        // 遍历每个三角形
        for (const auto &triangle: trianglesInput) {
            // 裁剪三角形，输出多边形顶点序列
            std::vector<geom::Vertex> clippedVertex;
            std::vector<maths::Vector3> clippedNormals;
            std::vector<Color> clippedColors;
            triangle.getVertex(clippedVertex, mesh);
            triangle.getVertNormals(clippedNormals, mesh);
            triangle.getVertColors(clippedColors);

            triangle.print();

            for (const auto &plane: planes) {
                clipVertices(plane, clippedVertex, clippedNormals, clippedColors);
            }

            // 检查三角形是不是全被剔除了
            if (clippedVertex.size() < 3) {
                std::clog << "triangle is fully clipped" << std::endl;
                continue;
            }

            // 组装新的三角形，采用三角形扇的方法
            // 计算可以生成多少个三角形
            size_t triangleNum = clippedVertex.size() - 2;

            // 组装三角形扇
            int vertStart = geom::Mesh::addNewVertex(clippedVertex[0], verticesOutput, vertChecker);
            int normStart = geom::Mesh::addNewNormal(clippedNormals[0], normalsOutput, normalChecker);
            Color &colStart = clippedColors[0];

            for (int i = 0; i < triangleNum; ++i) {
                int vertCur = geom::Mesh::addNewVertex(clippedVertex[i + 1], verticesOutput, vertChecker);
                int normCur = geom::Mesh::addNewNormal(clippedNormals[i + 1], normalsOutput, normalChecker);

                int vertNext = geom::Mesh::addNewVertex(clippedVertex[i + 2], verticesOutput, vertChecker);
                int normNext = geom::Mesh::addNewNormal(clippedNormals[i + 2], normalsOutput, normalChecker);

                Color &colCur = clippedColors[i + 1];
                Color &colNext = clippedColors[i + 2];

                std::clog << "add new triangle: " << vertStart << ", " << vertCur << ", " << vertNext << ", " << normStart << ", " <<
                        normCur << ", " << normNext << std::endl;

                trianglesOutput.emplace_back(vertStart, vertCur, vertNext,
                                             normStart, normCur, normNext,
                                             colStart, colCur, colNext);
            }
        }

        trianglesInput = trianglesOutput;
        verticesInput = verticesOutput;
        normalsInput = normalsOutput;
    }

    double threeDotCross(const maths::Vector2 &a, const maths::Vector2 &b, const maths::Vector2 &p) {
        const maths::Vector2 ab = b - a;
        const maths::Vector2 ap = p - a;
        return maths::Vector2::cross(ab, ap);
    }

    /**
     * 计算三角形表面的法线
     * @param vertices
     * @return
     */
    maths::Vector3 calcTriangleNormal(const std::vector<geom::Vertex> &vertices) {
        maths::Vector4 edge1 = vertices[1].pos - vertices[0].pos;
        maths::Vector4 edge2 = vertices[2].pos - vertices[1].pos;

        return maths::Vector4::cross(edge1, edge2).toVector3().normalize();
    }

    maths::Vector3 calcTriangleNormal(const geom::Vertex &a, const geom::Vertex &b, const geom::Vertex &c) {
        maths::Vector4 edge1 = b.pos - a.pos;
        maths::Vector4 edge2 = c.pos - b.pos;

        return maths::Vector4::cross(edge1, edge2).toVector3().normalize();
    }

    /**
     * 计算二维三角形有向面积的两倍
     * @param a
     * @param b
     * @param c
     * @return
     */
    double calcDoubleTriangleArea(const maths::Vector2 &a, const maths::Vector2 &b, const maths::Vector2 &c) {
        // area = (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y)
        return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
    }
}
