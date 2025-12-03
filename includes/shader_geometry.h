//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_SHADER_GEOMETRY_H
#define SOFTRENDERER_SHADER_GEOMETRY_H
#include <vector>

#include "Mesh.h"
#include "Rectangle.h"
#include "shader_render.h"
#include "Vector.h"
#include "Vertex.h"


namespace shader {
    const maths::Vector4 PLANE_NEAR(0, 0, 1, 1);

    const maths::Vector4 PLANE_FAR(0, 0, -1, 1);

    const maths::Vector4 PLANE_LEFT(1, 0, 0, 1);

    const maths::Vector4 PLANE_RIGHT(-1, 0, 0, 1);

    const maths::Vector4 PLANE_TOP(0, -1, 0, 1);

    const maths::Vector4 PLANE_BOTTOM(0, 1, 0, 1);

    bool isInsideTriangle(const geometry::Vertex vert[3], const maths::Vector2 &position);

    geometry::Rectangle calcTriangleBound(const geometry::Vertex vert[3]);

    std::vector<geometry::Vertex> clipVertices(const maths::Vector4 &plane,
                                               const std::vector<geometry::Vertex> &verticesInput);

    double calcDotOfPlane(const maths::Vector4 &plane, const maths::Vector4 &vec);

    /**
 * 三角形是否正方向面向摄像机
 * @param vertices
 * @return
 */
    bool isTriangleFacing(const std::vector<geometry::Vertex> &vertices);

    void clipMesh(geometry::Mesh &mesh);

    // static Vector4 calcIntersection(const Vector4 &p1, const Vector4 &p2, double t);

    double threeDotCross(const maths::Vector2 &a, const maths::Vector2 &b, const maths::Vector2 &p);

    maths::Vector3 calcTriangleNormal(const std::vector<geometry::Vertex> &vertices);

    maths::Vector3 calcTriangleNormal(const geometry::Vertex &a, const geometry::Vertex &b, const geometry::Vertex &c);

    double calcDoubleTriangleArea(const maths::Vector2 &a, const maths::Vector2 &b, const maths::Vector2 &c);
}

#endif //SOFTRENDERER_SHADER_GEOMETRY_H
