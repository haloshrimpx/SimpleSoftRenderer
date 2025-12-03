//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/shader_geometry.h"

namespace shader {
    bool isInsideTriangle(const geometry::Vertex vert[3], const maths::Vector2 &position) {
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

geometry::Rectangle calcTriangleBound(const geometry::Vertex vert[3]) {
    // 给定三个顶点 a b c
    // x min 就是 min(ax, bx, cx)
    // y min 就是 min(ay, by, cy)

    double xMax = vert[0].pos.x;
    double xMin = vert[0].pos.x;
    double yMax = vert[0].pos.y;
    double yMin = vert[0].pos.y;

    for (int i = 0; i < 3; ++i) {
        maths::Vector4 cur = vert[i].pos;
        xMin = std::min(cur.x, xMin);
        xMax = std::max(cur.x, xMax);
        yMin = std::min(cur.y, yMin);
        yMax = std::max(cur.y, yMax);
    }

    double width = xMax - xMin;
    double height = yMax - yMin;

    geometry::Rectangle res{
        {xMin, yMin},
        width, height,
    };

    return res;
}

std::vector<geometry::Vertex> clipVertices(const maths::Vector4 &plane,
                                           const std::vector<geometry::Vertex> &verticesInput) {
    // 空检查
    if (verticesInput.empty()) {
        return {};
    }

    std::vector<geometry::Vertex> output;
    size_t vertCount = verticesInput.size();
    std::vector<double> dotProducts(vertCount);

    // 预计算所有顶点与平面的点积
    for (size_t i = 0; i < vertCount; ++i) {
        dotProducts[i] = calcDotOfPlane(plane, verticesInput[i].pos);
    }

    for (size_t i = 0; i < vertCount; ++i) {
        const geometry::Vertex &currentVert = verticesInput[i];
        const geometry::Vertex &prevVert = verticesInput[(i - 1 + vertCount) % vertCount];

        double dCurrent = dotProducts[i];
        double dPrev = dotProducts[(i - 1 + vertCount) % vertCount];

        // 使用一个小的阈值来处理浮点数比较
        constexpr double epsilon = 1e-6;

        // 当前点在平面内
        if (dCurrent >= -epsilon) {
            if (dPrev < -epsilon) {
                // 有一个点在平面外，插值计算交点
                double t = dPrev / (dPrev - dCurrent);

                geometry::Vertex intersection = geometry::Vertex::lerp(prevVert, currentVert, t);
                output.push_back(intersection);
            }
            // 添加当前点
            output.push_back(currentVert);
        }
        // 前一个点在内部，计算插值点
        else if (dPrev >= -epsilon) {
            double t = dPrev / (dPrev - dCurrent);

            geometry::Vertex intersection = geometry::Vertex::lerp(prevVert, currentVert, t);
            output.push_back(intersection);
        }
    }

    return output;
}

double calcDotOfPlane(const maths::Vector4 &plane, const maths::Vector4 &vec) {
    return maths::Vector4::dot(plane, vec);
}

bool isTriangleFacing(const std::vector<geometry::Vertex> &vertices) {
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

void clipMesh(geometry::Mesh &mesh) {
    const maths::Vector4 planes[7] = {
        // PLANE_W,
        PLANE_NEAR,
        PLANE_FAR,
        PLANE_LEFT,
        PLANE_RIGHT,
        PLANE_TOP,
        PLANE_BOTTOM,
    };

    std::vector<geometry::Triangle> &trianglesInput = mesh.triangles;
    std::vector<geometry::Vertex> &verticesInput = mesh.vertices;

    std::vector<geometry::Triangle> trianglesOutput;
    std::vector<geometry::Vertex> verticesOutput;
    std::map<std::string, int> checker; // 三角索引网格的检查

    // 遍历每个三角形
    for (const auto &triangle: trianglesInput) {
        // 裁剪三角形，输出多边形顶点序列
        std::vector<geometry::Vertex> clippedVertex;
        triangle.getVertex(clippedVertex, mesh);


        for (const auto &plane: planes) {
            clippedVertex = clipVertices(plane, clippedVertex);
        }

        // 检查三角形是不是全被剔除了
        if (clippedVertex.size() < 3)
            continue;

        // 组装新的三角形，采用三角形扇的方法
        // 计算可以生成多少个三角形
        size_t triangleNum = clippedVertex.size() - 2;

        int iStart = geometry::Mesh::addNewVertex(clippedVertex[0], verticesOutput, checker);

        for (int i = 0; i < triangleNum; ++i) {
            int iCur = geometry::Mesh::addNewVertex(clippedVertex[i + 1], verticesOutput, checker);
            int iNext = geometry::Mesh::addNewVertex(clippedVertex[i + 2], verticesOutput, checker);

            trianglesOutput.emplace_back(iStart, iCur, iNext, triangle.faceNormal);
        }
    }

    trianglesInput = trianglesOutput;
    verticesInput = verticesOutput;
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
maths::Vector3 calcTriangleNormal(const std::vector<geometry::Vertex> &vertices) {
    maths::Vector4 edge1 = vertices[1].pos - vertices[0].pos;
    maths::Vector4 edge2 = vertices[2].pos - vertices[1].pos;

    return maths::Vector4::cross(edge1, edge2).toVector3().normalize();
}

maths::Vector3 calcTriangleNormal(const geometry::Vertex &a, const geometry::Vertex &b, const geometry::Vertex &c) {
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
