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

    void shadingVertex(Object &object, const DirectionalLight &dirLight, const maths::Vector3 &camPos,
                       const Color &ambient) {
        geometry::Mesh &mesh = object.mesh;

        for (auto &vertex: mesh.vertices) {
            // 为顶点计算光照
            maths::Vector3 &normal = vertex.vertNormal;
            std::cout << "before: ";
            maths::Vector3 viewDir = vertex.pos.toVector3() - camPos;
            maths::Vector3 lightDir = {1, 1, 1};
            Color spec = calcSpecular(object.material.baseColor, dirLight.color, normal, object.material.gloss,
                                      lightDir, viewDir);
            Color diff = calcDiffuse(object.material.baseColor, dirLight.color, normal,
                                     lightDir);
            vertex.vertColor = spec + diff + ambient;
        }
    }

    maths::Vector4 getTriangleWeight(const geometry::Vertex triangle[3], const maths::Vector2 &p) {
        const maths::Vector2 &A = triangle[0].pos.toVector2();
        const maths::Vector2 &B = triangle[1].pos.toVector2();
        const maths::Vector2 &C = triangle[2].pos.toVector2();
        double area = calcDoubleTriangleArea(A, B, C);

        // 权重(alpha, beta, gamma), 三者和为1
        double alpha = calcDoubleTriangleArea(p, B, C) / area;
        double beta = calcDoubleTriangleArea(A, p, C) / area;
        double gamma = 1 - alpha - beta;
        double flag = (alpha < 0 || beta < 0 || gamma < 0) ? -1 : 0;
        return {alpha, beta, gamma, flag};
    }

    maths::Vector4 getTriangleWeight(const geometry::Vertex triangle[3], const double area, const maths::Vector2 &p) {
        const maths::Vector2 &A = triangle[0].pos.toVector2();
        const maths::Vector2 &B = triangle[1].pos.toVector2();
        const maths::Vector2 &C = triangle[2].pos.toVector2();

        // 权重(alpha, beta, gamma), 三者和为1
        double alpha = calcDoubleTriangleArea(p, B, C) / area;
        double beta = calcDoubleTriangleArea(A, p, C) / area;
        double gamma = 1 - alpha - beta;
        double flag = (alpha < 0 || beta < 0 || gamma < 0) ? -1 : 0;
        return {alpha, beta, gamma, flag};
    }


    /**
     * 透视校正插值任何属性
     * 默认点在三角形内
     *
     * @param triangle 三角形的三个顶点
     * @param weight 插值点的重心权重
     * @param iA 顶点A的属性
     * @param iB 顶点B的属性
     * @param iC 顶点C的属性
     * @return
     */
    double perspCorrectionLerp(const geometry::Vertex triangle[3],
                               const maths::Vector4 &weight,
                               const double iA, const double iB, const double iC) {
        double alpha = weight.x;
        double beta = weight.y;
        double gamma = weight.z;

        double zA = triangle[0].pos.z;
        double zB = triangle[1].pos.z;
        double zC = triangle[2].pos.z;
        double zP = 1 / (alpha / zA + beta / zB + gamma / zC);

        // Ip = (alpha * iA / zA + beta * iB / zB + gamma * iC / zC) / zP
        return (alpha * iA / zA + beta * iB / zB + gamma * iC / zC) / zP;
    }

    maths::Vector3 perspCorrectionLerp(const geometry::Vertex triangle[3],
                                       const maths::Vector4 &weight,
                                       const maths::Vector3 &iA, const maths::Vector3 &iB, const maths::Vector3 &iC) {
        double alpha = weight.x;
        double beta = weight.y;
        double gamma = weight.z;

        double zA = triangle[0].pos.z;
        double zB = triangle[1].pos.z;
        double zC = triangle[2].pos.z;
        double zP = 1 / (alpha / zA + beta / zB + gamma / zC);

        // Ip = (alpha * iA / zA + beta * iB / zB + gamma * iC / zC) / zP
        return (alpha * iA * (1 / zA) + beta * iB * (1 / zB) + gamma * iC * (1 / zC)) * (1 / zP);
    }


    Color lerpTriangleColor(const geometry::Vertex triangle[3], const maths::Vector2 &p) {
        // 权重(alpha, beta, gamma), 三者和为1
        maths::Vector4 weight = getTriangleWeight(triangle, p);
        double alpha = weight.x;
        double beta = weight.y;
        double gamma = weight.z;

        // 如果点在三角形外
        if (weight.w < 0) {
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
        maths::Vector4 weight = getTriangleWeight(vertices, p);
        if (weight.w < 0)
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
        Color pixColor = lerpTriangleColor(vertices, p).toSRGBColor().sRGBClamp();

        // 判断点是否在面内
        if (pixColor.a < 0)
            return;

        int x = std::round(p.x);
        int y = std::round(p.y);
        putpixel(x, y, RGB(pixColor.r, pixColor.g, pixColor.b));
    }

    /**
     * 对像素插值顶点法线，并且映射到颜色
     * @param vertices
     * @param p
     */
    void renderVertexNormal(geometry::Vertex vertices[3], const maths::Vector2 &p) {
        maths::Vector4 weight = getTriangleWeight(vertices, p);
        if (weight.w < 0)
            return;

        maths::Vector3 normal = perspCorrectionLerp(vertices, weight,
                                                    vertices[0].vertNormal,
                                                    vertices[1].vertNormal,
                                                    vertices[2].vertNormal).normalize() * 255;

        int x = std::round(p.x);
        int y = std::round(p.y);
        putpixel(x, y, RGB(normal.x, normal.y, normal.z));
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
