//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/shader_render.h"

#include <chrono>
#include <cmath>
#include <graphics.h>
#include <iostream>

#include "../includes/Buffer.h"
#include "../includes/Camera.h"
#include "../includes/shader_geometry.h"
#include "../includes/shader_transform.h"
#include "../includes/Light.h"
#include "../includes/Object.h"

namespace shader {
    void renderBuffer(const Buffer &buffer) {
        const maths::Vector2 size = buffer.getBufferSize();
        const Color *colorBuffer = buffer.getColorBuffer();
        // 行
        for (int i = 0; i < size.x; ++i) {
            for (int j = 0; j < size.y; ++j) {
                // 列
                Color pixel = colorBuffer[buffer.getIndex(i, j)];
                pixel.print();
                putpixel(i, j, RGB(pixel.r, pixel.g, pixel.b));
            }
        }
    }

    void vertexShadingPipeline(const Object &renderObject, const DirectionalLight &light, const Camera &camera,
                               const Buffer &buffer, const int screenWidth, const int screenHeight) {
        Object renderCube = renderObject;
        Camera renderCam = camera;
        DirectionalLight sun = light;

        transformObjToWorldSpace(renderCube);

        shadingVertex(renderCube, sun, renderCam.transform.getPosition(), Color(0.1, 0.1, 0.1, 1));

        transformObjToViewSpace(renderCube, renderCam);

        backFaceCulling(renderCube);

        transformObjToPerspProjSpace(renderCube, renderCam);

        std::clog << ">>>>>>>>>>>PROJECTION SPACE" << std::endl;
        renderCube.mesh.print();

        clipMesh(renderCube.mesh);

        std::clog << ">>>>>>>>>>>MESH CLIPPING" << std::endl;
        renderCube.mesh.print();

        applyPerspectiveDivision(renderCube);

        std::clog << ">>>>>>>>>>>PERSPECTIVE DIVISION" << std::endl;
        renderCube.mesh.print();

        transformObjToViewportSpace(renderCube, screenWidth, screenHeight);

        std::clog << ">>>>>>>>>>>VIEWPORT SPACE" << std::endl;
        renderCube.mesh.print();

        // 开始光栅化
        cleardevice();
        BeginBatchDraw();

        // 清除缓冲区
        buffer.clear();
        rasterizeObject(renderCube, buffer);

        std::clog << ">>>>>>>>>>>RASTERIZATION" << std::endl;
        renderCube.mesh.print();

        renderBuffer(buffer);

        // for (auto &triangle: renderCube.mesh.triangles) {
        //     geom::Vertex vertices[3];
        //     triangle.getVertex(vertices, renderCube.mesh);
        //     renderTriangleWireframe(vertices);
        // }

        EndBatchDraw();
    }

    void rasterizeObject(const Object &object, const Buffer &buffer) {
        const geom::Mesh &mesh = object.mesh;
        const size_t triangleCount = mesh.triangles.size();

        // 遍历所有的三角形
        for (int i = 0; i < triangleCount; ++i) {
            geom::Vertex vertices[3];
            mesh.triangles[i].getVertex(vertices, mesh);

            // 计算包围盒
            geom::Rectangle rect = calcTriangleBound(vertices);
            maths::Vector2 startPoint = rect.getOriginPoint();

            // 光栅化
            for (int j = 0; j < rect.height; ++j) {
                for (int k = 0; k < rect.width; ++k) {
                    maths::Vector2 pos = {startPoint.x + k, startPoint.y + j};
                    renderPixelColor(mesh.triangles[i], mesh, pos, buffer);
                }
            }
        }
    }

    void shadingVertex(Object &object, const DirectionalLight &dirLight, const maths::Vector3 &camPos,
                       const Color &ambient) {
        geom::Mesh &mesh = object.mesh;
        maths::Vector3 lightDir = {1, 1, 1}; // FOR DEBUG

        for (auto &triangle: mesh.triangles) {
            std::vector<geom::Vertex> vertices;
            std::vector<maths::Vector3> normals;
            triangle.getVertNormals(normals, mesh);
            triangle.getVertex(vertices, mesh);

            for (int i = 0; i < 3; ++i) {
                // 为顶点计算光照
                maths::Vector3 &normal = normals[i];
                geom::Vertex &vertex = vertices[i];

                maths::Vector3 viewDir = vertex.pos.toVector3() - camPos;

                Color spec = calcSpecular(object.material.baseColor, dirLight.color, normal, object.material.gloss,
                                          lightDir, viewDir);
                Color diff = calcDiffuse(object.material.baseColor, dirLight.color, normal,
                                         lightDir);
                triangle.vertColors[i] = spec + diff + ambient; // 将光照信息存储进三角形
            }
        }
    }

    void shadingFace(Object &object, const DirectionalLight &dirLight) {
        // TODO
    }

    maths::Vector4 getTriangleWeight(const geom::Vertex triangle[3], const maths::Vector2 &p) {
        const maths::Vector2 &A = triangle[0].pos.toVector2();
        const maths::Vector2 &B = triangle[1].pos.toVector2();
        const maths::Vector2 &C = triangle[2].pos.toVector2();
        double area = calcDoubleTriangleArea(A, B, C);

        // 权重(alpha, beta, gamma), 三者和为1
        double alpha = calcDoubleTriangleArea(p, B, C) / area;
        double beta = calcDoubleTriangleArea(A, p, C) / area;
        double gamma = 1 - alpha - beta;

        // double inv = 1/(alpha + beta + gamma);
        // alpha *= inv;
        // beta *= inv;
        // gamma *= inv;

        double flag = (alpha < 0 || beta < 0 || gamma < 0) ? -1 : 0;
        return {alpha, beta, gamma, flag};
    }

    maths::Vector4 getTriangleWeight(const geom::Vertex triangle[3], const double area, const maths::Vector2 &p) {
        const maths::Vector2 &A = triangle[0].pos.toVector2();
        const maths::Vector2 &B = triangle[1].pos.toVector2();
        const maths::Vector2 &C = triangle[2].pos.toVector2();

        // 权重(alpha, beta, gamma), 三者和为1
        double alpha = calcDoubleTriangleArea(p, B, C) / area;
        double beta = calcDoubleTriangleArea(A, p, C) / area;
        double gamma = 1 - alpha - beta;

        // double inv = 1/(alpha + beta + gamma);
        // alpha *= inv;
        // beta *= inv;
        // gamma *= inv;
        //
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
    double perspCorrectionLerp(const geom::Vertex triangle[3],
                               const maths::Vector4 &weight,
                               const double iA, const double iB, const double iC) {
        double alpha = weight.x;
        double beta = weight.y;
        double gamma = weight.z;

        double zA = triangle[0].pos.z;
        double zB = triangle[1].pos.z;
        double zC = triangle[2].pos.z;
        double zP = perspCorrectionLerpZ(triangle, weight);

        // Ip = (alpha * iA / zA + beta * iB / zB + gamma * iC / zC) / zP
        return (alpha * iA / zA + beta * iB / zB + gamma * iC / zC) / zP;
    }

    maths::Vector3 perspCorrectionLerp(const geom::Vertex triangle[3],
                                       const maths::Vector4 &weight,
                                       const maths::Vector3 &iA, const maths::Vector3 &iB, const maths::Vector3 &iC) {
        double alpha = weight.x;
        double beta = weight.y;
        double gamma = weight.z;

        double zA = triangle[0].pos.z;
        double zB = triangle[1].pos.z;
        double zC = triangle[2].pos.z;
        double zP = perspCorrectionLerpZ(triangle, weight);

        // Ip = (alpha * iA / zA + beta * iB / zB + gamma * iC / zC) / zP
        return (alpha * iA * (1 / zA) + beta * iB * (1 / zB) + gamma * iC * (1 / zC)) * (1 / zP);
    }

    maths::Vector4 perspCorrectionLerp(const geom::Vertex triangle[3],
                                       const maths::Vector4 &weight,
                                       const maths::Vector4 &iA, const maths::Vector4 &iB, const maths::Vector4 &iC) {
        double alpha = weight.x;
        double beta = weight.y;
        double gamma = weight.z;

        double zA = triangle[0].pos.z;
        double zB = triangle[1].pos.z;
        double zC = triangle[2].pos.z;
        double zP = perspCorrectionLerpZ(triangle, weight);

        // Ip = (alpha * iA / zA + beta * iB / zB + gamma * iC / zC) / zP
        return (alpha * iA * (1 / zA) + beta * iB * (1 / zB) + gamma * iC * (1 / zC)) * (1 / zP);
    }

    /**
     * 透视矫正插值深度
     * @param triangle
     * @param weight
     * @return
     */
    double perspCorrectionLerpZ(const geom::Vertex triangle[3], const maths::Vector4 &weight) {
        double zA = triangle[0].pos.z;
        double zB = triangle[1].pos.z;
        double zC = triangle[2].pos.z;
        const maths::Vector3 depths = {zA, zB, zC};
        double depth = maths::Vector3::dot(depths, weight.toVector3());

        return depth;
    }

    Color lerpTriangleColor(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &p, const maths::Vector4 &weight,
                            const double z) {
        double alpha = weight.x;
        double beta = weight.y;
        double gamma = weight.z;

        // 如果点在三角形外
        if (weight.w < 0) {
            return COLOR_INVALID;
        }

        geom::Vertex vertices[3];
        triangle.getVertex(vertices, mesh);

        // 插值深度
        // const double z = alpha * triangle[0].pos.z + beta * triangle[1].pos.z + gamma * triangle[2].pos.z;

        // 插值颜色
        const Color &colA = triangle.vertColors[0] * (1 / vertices[0].pos.z) * alpha;
        const Color &colB = triangle.vertColors[1] * (1 / vertices[1].pos.z) * beta;
        const Color &colC = triangle.vertColors[2] * (1 / vertices[2].pos.z) * gamma;

        Color res = (colA + colB + colC) * (1 / z);
        res.a = 1;
        return res;
    }

    /**
     * 根据顶点插值渲染单个像素的深度
     * 如果像素位置在三角形外，就不渲染
     * @param triangle
     * @param mesh
     * @param scrPos
     * @param buffer
     */
    void renderPixelDepth(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                          const Buffer &buffer) {
        geom::Vertex vertices[3];
        triangle.getVertex(vertices, mesh);

        maths::Vector4 weight = getTriangleWeight(vertices, scrPos);
        if (weight.w < 0)
            return;

        double z = perspCorrectionLerpZ(vertices, weight);
        Color pixCol(z, z, z);

        // int x = std::round(scrPos.x);
        // int y = std::round(scrPos.y);

        // 四舍五入
        int x = (int) std::round(scrPos.x + 0.5);
        int y = (int) std::round(scrPos.y + 0.5);


        buffer.writeColorBuffer(x, y, pixCol.toSRGBColor().sRGBClamp(), z);
        // putpixel(x, y, RGB(z, z, z));
    }

    /**
     * 逐像素插值顶点颜色
     * 如果像素位置在三角形外，就不渲染
     * @param triangle
     * @param mesh
     * @param scrPos
     * @param buffer
     */
    void renderPixelColor(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                          const Buffer &buffer) {
        geom::Vertex vertices[3];
        triangle.getVertex(vertices, mesh);

        maths::Vector4 weight = getTriangleWeight(vertices, scrPos);
        // 判断点是否在面内
        if (weight.w < 0)
            return;

        double z = perspCorrectionLerpZ(vertices, weight);
        Color pixCol = lerpTriangleColor(triangle, mesh, scrPos, weight, z).toSRGBColor().sRGBClamp(); // HERE

        // pixCol.print();

        int x = (int) std::round(scrPos.x + 0.5);
        int y = (int) std::round(scrPos.y + 0.5);

        buffer.writeColorBuffer(x, y, pixCol, z);
        // int x = std::round(p.x);
        // int y = std::round(p.y);
        // putpixel(x, y, RGB(pixColor.r, pixColor.g, pixColor.b));
    }

    /**
     * 对像素插值顶点法线，并且映射到颜色
     * @param triangle
     * @param mesh
     * @param scrPos
     * @param buffer
     */
    void renderVertexNormal(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                            const Buffer &buffer) {
        geom::Vertex vertices[3];
        maths::Vector3 normals[3];
        triangle.getVertNormals(normals, mesh);
        triangle.getVertex(vertices, mesh);

        maths::Vector4 weight = getTriangleWeight(vertices, scrPos);
        if (weight.w < 0)
            return;

        double z = perspCorrectionLerpZ(vertices, weight);
        maths::Vector3 normal = perspCorrectionLerp(vertices, weight, normals[0], normals[1], normals[2]).normalize() * 255;
        int x = std::round(scrPos.x);
        int y = std::round(scrPos.y);

        buffer.writeColorBuffer(x, y, {
                                    static_cast<float>(normal.x), static_cast<float>(normal.y), static_cast<float>(normal.z)
                                }, z);
    }

    /**
     * 绘制三角形线框
     * @param vertices
     */
    void renderTriangleWireframe(geom::Vertex vertices[3]) {
        // 绘制线框
        for (int p = 0; p < 3; ++p) {
            maths::Vector2 start = vertices[p].pos.toVector2();
            maths::Vector2 end = vertices[(p + 1) % 3].pos.toVector2();

            std::clog << "render line" << std::endl;
            line(start.x, start.y, end.x, end.y);
        }
    }
}
