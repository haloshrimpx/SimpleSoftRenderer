//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/shader_render.h"

#include <chrono>
#include <cmath>
#include <graphics.h>
#include <iostream>

#include "../includes/shader_geometry.h"
#include "../includes/Light.h"
#include "../includes/Object.h"

namespace shader {
    void rasterizeObject(const Object &object) {
        const geometry::Mesh &mesh = object.mesh;
        const size_t triangleCount = mesh.triangles.size();

        // 遍历所有的三角形
        for (int i = 0; i < triangleCount; ++i) {
            geometry::Vertex vertices[3];
            mesh.triangles[i].getVertex(vertices, mesh);

            geometry::Rectangle rect = calcTriangleBound(vertices);
            maths::Vector2 startPoint = rect.originPoint;

            // 向上取整
            int height = round(rect.height);
            int width = round(rect.width);

            // 光栅化
            for (int j = 0; j < height; ++j) {
                for (int k = 0; k < width; ++k) {
                    maths::Vector2 pos = {startPoint.x + k, startPoint.y + j};
                    renderPixelColor(vertices, pos);
                }
            }

            renderTriangleWireframe(vertices);
        }
    }

    void shadingVertex(Object &object, const DirectionalLight &dirLight) {
        geometry::Mesh &mesh = object.mesh;

        for (auto &vertex: mesh.vertices) {
            // 为顶点计算光照
            maths::Vector3 &normal = vertex.vertNormal;
            std::cout << "before: ";

            vertex.vertColor = calcDiffuse(vertex.vertColor, dirLight.color, normal,
                                           {1, 1, 1});
        }
    }

    maths::Vector3 getTriangleWeight(const geometry::Vertex triangle[3], const maths::Vector2 &p) {
        const maths::Vector2 &A = triangle[0].pos.toVector2();
        const maths::Vector2 &B = triangle[1].pos.toVector2();
        const maths::Vector2 &C = triangle[2].pos.toVector2();
        double area = calcDoubleTriangleArea(A, B, C);

        // 权重(alpha, beta, gamma), 三者和为1
        double alpha = calcDoubleTriangleArea(p, B, C) / area;
        double beta = calcDoubleTriangleArea(A, p, C) / area;
        double gamma = 1 - alpha - beta;
        return {alpha, beta, gamma};
    }

    maths::Vector3 getTriangleWeight(const geometry::Vertex triangle[3], const double area, const maths::Vector2 &p) {
        const maths::Vector2 &A = triangle[0].pos.toVector2();
        const maths::Vector2 &B = triangle[1].pos.toVector2();
        const maths::Vector2 &C = triangle[2].pos.toVector2();

        // 权重(alpha, beta, gamma), 三者和为1
        double alpha = calcDoubleTriangleArea(p, B, C) / area;
        double beta = calcDoubleTriangleArea(A, p, C) / area;
        double gamma = 1 - alpha - beta;
        return {alpha, beta, gamma};
    }


    Color lerpTriangleColor(const geometry::Vertex triangle[3], const maths::Vector2 &p) {
        // 权重(alpha, beta, gamma), 三者和为1
        maths::Vector3 weight = getTriangleWeight(triangle, p);
        double alpha = weight.x;
        double beta = weight.y;
        double gamma = weight.z;

        // 如果点在三角形外
        if (alpha < 0 || beta < 0 || weight.z < 0) {
            return COLOR_INVALID;
        }
        // 顶点颜色的透视校正

        // 插值深度
        const double z = alpha * triangle[0].pos.z + beta * triangle[1].pos.z + gamma * triangle[2].pos.z;

        // 插值颜色
        const Color &colA = triangle[0].vertColor * (1 / triangle[0].pos.z) * alpha;
        const Color &colB = triangle[1].vertColor * (1 / triangle[1].pos.z) * beta;
        const Color &colC = triangle[2].vertColor * (1 / triangle[2].pos.z) * gamma;

        Color res = (colA + colB + colC) * (1 / z);
        res.a = 1;
        return res;
    }

    /**
     * 根据顶点插值渲染单个像素的深度
     * 如果像素位置在三角形外，就不渲染
     * @param vertices
     * @param p
     */
    void renderPixelDepth(geometry::Vertex vertices[3], const maths::Vector2 &p) {
        maths::Vector3 weight = getTriangleWeight(vertices, p);
        if (weight.x < 0 || weight.y < 0 || weight.z < 0)
            return;
        double z = Color::mapLinearToSRGB(1 - (weight.x * vertices[0].pos.z
                                               + weight.y * vertices[1].pos.z
                                               + weight.z * vertices[2].pos.z));
        int x = std::round(p.x);
        int y = std::round(p.y);
        putpixel(x, y, RGB(z, z, z));
    }

    /**
     * 逐像素插值顶点颜色
     * 如果像素位置在三角形外，就不渲染
     * @param vertices
     * @param p
     */
    void renderPixelColor(geometry::Vertex vertices[3], const maths::Vector2 &p) {
        Color pixColor = lerpTriangleColor(vertices, p).toSRGBColor();

        // 判断点是否在面内
        if (pixColor.a < 0)
            return;

        int x = std::round(p.x);
        int y = std::round(p.y);
        putpixel(x, y, RGB(pixColor.r, pixColor.g, pixColor.b));
    }

    /**
     * 绘制三角形线框
     * @param vertices
     */
    void renderTriangleWireframe(geometry::Vertex vertices[3]) {
        // 绘制线框
        for (int p = 0; p < 3; ++p) {
            maths::Vector2 start = vertices[p].pos.toVector2();
            maths::Vector2 end = vertices[(p + 1) % 3].pos.toVector2();
            std::clog << "draw line: " << std::endl;
            start.print();
            end.print();
            line(start.x, start.y, end.x, end.y);
            // drawLine(start, end);
        }
    }
}
