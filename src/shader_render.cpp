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
#include  "../includes/Matrix.h"
#include "../includes/Object.h"

namespace shader {
    void renderBuffer(const ScreenBuffer &buffer) {
        const maths::Vector2 size = {static_cast<double>(buffer.getWidth()), static_cast<double>(buffer.getHeight())};
        const Color *colorBuffer = buffer.getColorBuffer();
        // 行
        for (int i = 0; i < size.x; ++i) {
            for (int j = 0; j < size.y; ++j) {
                // 列
                Color pixel = colorBuffer[buffer.getIndex(i, j)];

                putpixel(i, j, RGB(pixel.r, pixel.g, pixel.b));
            }
        }
    }

    void renderDepthBuffer(const DepthBuffer &buffer) {
        const maths::Vector2 size = {static_cast<double>(buffer.getWidth()), static_cast<double>(buffer.getHeight())};
        // 行
        for (int i = 0; i < size.x; ++i) {
            for (int j = 0; j < size.y; ++j) {
                // 列
                double z = buffer.getDepth(i, j);

                z = z * 2 - 1; // 映射到[-1, 1]
                // 线性深度
                double n = 0.5;
                double f = 100;
                double linearDepth = (2 * n * f) / (f + n - z * (f - n));

                double normalized = (linearDepth - n) / (f - n);

                // 反转颜色
                auto gray = std::clamp(1 - static_cast<float>(normalized), 0.0f, 1.0f) * 255;
                putpixel(i, j, RGB(gray, gray, gray));
            }
        }
    }

    void gouraudShadingPipeline(const std::vector<Object> &objects,
                                const std::vector<std::unique_ptr<Light> > &lights,
                                const Camera &camera, const ScreenBuffer &buffer) {
        std::vector<Object> renderObjects = objects;
        const Camera &renderCam = camera;

        // DEBUG 点光源的阴影
        DepthBuffer depthBuffer(1024, 1024);
        Camera lightCamera(90, 0.05, 1000, 1024, 1024, {
                               {0, 4, 4}, {-60, 0, 0}, {1, 1, 1}
                           });
        maths::Matrix4x4 lightVPMatr = getPerspVPMatrix(lightCamera, lightCamera.transform);
        std::cerr << "light vp" << std::endl;
        lightVPMatr.print();
        renderShadowBuffer(renderObjects, lightCamera, depthBuffer);

        // 清空缓冲区
        cleardevice();
        buffer.clear();

        for (auto &obj: renderObjects) {
            transformObjToWorldSpace(obj);

            shadingVertex(obj, lights, renderCam.transform.getPosition(), Color(0.08, 0.08, 0.08, 1));

            transformObjToViewSpace(obj, renderCam.transform);

            cullingFaces(obj, CullingMode::BACK);

            transformObjToPerspProjSpace(obj, renderCam);

            std::clog << ">>>>>>>>>>>PROJECTION SPACE" << std::endl;
            obj.mesh.print();

            clipMesh(obj.mesh);

            std::clog << ">>>>>>>>>>>MESH CLIPPING" << std::endl;
            obj.mesh.print();

            applyPerspectiveDivision(obj);

            std::clog << ">>>>>>>>>>>PERSPECTIVE DIVISION" << std::endl;
            obj.mesh.print();

            transformObjToViewportSpace(obj, renderCam.windowWidth, renderCam.windowHeight);

            std::clog << ">>>>>>>>>>>VIEWPORT SPACE" << std::endl;
            obj.mesh.print();

            maths::Matrix4x4 invVPMatr =
                    getPerspVPMatrix(renderCam, renderCam.transform).inverse();

            rasterizeObject(obj, buffer, invVPMatr, lightVPMatr, depthBuffer);

            std::clog << ">>>>>>>>>>>RASTERIZATION" << std::endl;
            obj.mesh.print();
            //
            // for (auto &triangle: obj.mesh.triangles) {
            //     geom::Vertex vertices[3];
            //     triangle.getVertex(vertices, obj.mesh);
            //     renderTriangleWireframe(vertices);
            // }
        }
        // 绘制物体
        BeginBatchDraw();

        // renderDepthBuffer(depthBuffer);
        renderBuffer(buffer);

        EndBatchDraw();
    }

    void renderShadowBuffer(const std::vector<Object> &objects, const Camera &lightCamera, DepthBuffer &depthBuffer) {
        // 对顶点进行变换，并光栅化深度，写入缓冲区
        std::vector<Object> _objects = objects;

        for (auto &obj: _objects) {
            transformObjToWorldSpace(obj); // M
            transformObjToViewSpace(obj, lightCamera.transform); // MV
            cullingFaces(obj, CullingMode::FRONT);
            transformObjToPerspProjSpace(obj, lightCamera); // MVP
            clipMesh(obj.mesh);
            applyPerspectiveDivision(obj);
            transformObjToViewportSpace(obj, lightCamera.windowWidth, lightCamera.windowHeight);

            // 光栅化深度
            const geom::Mesh &mesh = obj.mesh;
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
                        writePerspPixelDepth(mesh.triangles[i], mesh, pos, depthBuffer);
                    }
                }
            }
        }
    }

    void rasterizeObject(const Object &object, const ScreenBuffer &scrBuffer,
                         const maths::Matrix4x4 &invVPMatr, const maths::Matrix4x4 &lightVPMatr,
                         const DepthBuffer &depthBuffer) {
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
                    // renderPerspPixelDepth(mesh.triangles[i], mesh, pos, buffer, 0.5, 100);
                    // renderOrthoPixelDepth(mesh.triangles[i], mesh, pos, buffer);
                    renderPixelColor(mesh.triangles[i], mesh, pos, invVPMatr, lightVPMatr, scrBuffer, depthBuffer);
                }
            }
        }
    }

    void shadingVertex(Object &object, const std::vector<std::unique_ptr<Light> > &lights, const maths::Vector3 &camPos,
                       const Color &ambient) {
        // 遍历光源列表，判断类型
        // 获取：光源颜色，强度，相对顶点的方向

        // 计算世界空间光照
        geom::Mesh &mesh = object.mesh;

        // 计算各种光源的照明
        for (auto &light: lights) {
            if (light->getType() == LightType::DIRECTIONAL) {
                // 方向光
                auto *dirLight = dynamic_cast<DirectionalLight *>(light.get());
                handleDirectionalLightPersp(object, dirLight, camPos);
            } else if (light->getType() == LightType::POINT) {
                // 点光源
                auto *pointLight = dynamic_cast<PointLight *>(light.get());
                handlePointLightPersp(object, pointLight, camPos);
            }
        }

        // 计算环境光
        for (auto &triangle: mesh.triangles) {
            for (auto &vertColor: triangle.vertColors) {
                // 为顶点计算环境光
                vertColor = vertColor + ambient; // 将环境光信息存储进三角形
            }
        }
    }

    void handlePointLightPersp(Object &object, const PointLight *pointLight, const maths::Vector3 &camPos) {
        maths::Vector3 lightPos = pointLight->transform.getPosition();
        maths::Vector3 lightDir;
        Color lightColor = pointLight->color * pointLight->intensity;

        for (auto &triangle: object.mesh.triangles) {
            std::vector<geom::Vertex> vertices;
            std::vector<maths::Vector3> normals;
            triangle.getVertNormals(normals, object.mesh);
            triangle.getVertex(vertices, object.mesh);

            for (int i = 0; i < 3; ++i) {
                // 为顶点计算光照
                maths::Vector3 &normal = normals[i];
                geom::Vertex &vertex = vertices[i];
                lightDir = lightPos - vertices[i].pos.toVector3();

                maths::Vector3 viewDir = vertex.pos.toVector3() - camPos;

                Color spec = calcSpecular(object.material.baseColor, pointLight->color, normal, object.material.gloss,
                                          lightDir, viewDir);

                Color diff = calcDiffuse(object.material.baseColor, pointLight->color, normal,
                                         lightDir);

                triangle.vertColors[i] = triangle.vertColors[i] + diff + spec; // 将光照信息存储进三角形
            }
        }
    }

    /**
     * 计算方向光的光照
     * @param object
     * @param dirLight
     * @param camPos
     */
    void handleDirectionalLightPersp(Object &object, const DirectionalLight *dirLight, const maths::Vector3 &camPos) {
        maths::Vector3 lightDir = dirLight->getDirection();
        Color lightColor = dirLight->color * dirLight->intensity;

        for (auto &triangle: object.mesh.triangles) {
            std::vector<geom::Vertex> vertices;
            std::vector<maths::Vector3> normals;
            triangle.getVertNormals(normals, object.mesh);
            triangle.getVertex(vertices, object.mesh);

            for (int i = 0; i < 3; ++i) {
                // 为顶点计算光照
                maths::Vector3 &normal = normals[i];
                geom::Vertex &vertex = vertices[i];

                maths::Vector3 viewDir = vertex.pos.toVector3() - camPos;

                Color spec = calcSpecular(object.material.baseColor, dirLight->color, normal, object.material.gloss,
                                          lightDir, viewDir);

                Color diff = calcDiffuse(object.material.baseColor, dirLight->color, normal,
                                         lightDir);

                triangle.vertColors[i] = triangle.vertColors[i] + diff + spec; // 将光照信息存储进三角形
            }
        }
    }

    void shadingFace(Object &object, const std::vector<std::unique_ptr<Light> > &lights) {
        // TODO
    }

    /**
     *
     * @param triangle
     * @param p
     * @return
     */
    maths::Vector4 calcTriangleWeight(const geom::Vertex triangle[3], const maths::Vector2 &p) {
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

    maths::Vector4 calcTriangleWeight(const geom::Vertex triangle[3], const double area, const maths::Vector2 &p) {
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

        double invZA = 1.0 / zA;
        double invZB = 1.0 / zB;
        double invZC = 1.0 / zC;

        double invDepth = maths::Vector3::dot({invZA, invZB, invZC}, weight.toVector3());
        double depth = 1.0 / invDepth;

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

        // 插值颜色
        const Color &colA = triangle.vertColors[0] * (1 / vertices[0].pos.z) * alpha;
        const Color &colB = triangle.vertColors[1] * (1 / vertices[1].pos.z) * beta;
        const Color &colC = triangle.vertColors[2] * (1 / vertices[2].pos.z) * gamma;

        Color res = (colA + colB + colC) * (1 / z);
        res.a = 1;
        return res;
    }

    /**
     * 根据顶点插值渲染单个像素的线性深度
     * 如果像素位置在三角形外，就不渲染
     * @param triangle
     * @param mesh
     * @param scrPos
     * @param buffer
     * @param n 近平面
     * @param f 远平面
     */
    void renderPerspPixelDepth(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                               const ScreenBuffer &buffer, const float n, const float f) {
        geom::Vertex vertices[3];
        triangle.getVertex(vertices, mesh);

        maths::Vector4 weight = calcTriangleWeight(vertices, scrPos);
        if (weight.w < 0)
            return;

        double z = perspCorrectionLerpZ(vertices, weight);
        z = z * 2 - 1; // 映射到[-1, 1]
        // 线性深度
        double linearDepth = (2 * n * f) / (f + n - z * (f - n)); //(2 * _near * 100) / (100 + 0.5 - z * (100 - 0.5));

        double normalized = (linearDepth - n) / (f - n);

        // 反转颜色
        auto gray = 1 - static_cast<float>(normalized);

        Color pixCol(gray, gray, gray);

        // 四舍五入
        int x = (int) std::round(scrPos.x + 0.5);
        int y = (int) std::round(scrPos.y + 0.5);


        buffer.writeColorBuffer(x, y, pixCol.toSRGBColor().sRGBClamp(), z);
        // putpixel(x, y, RGB(z, z, z));
    }

    void writePerspPixelDepth(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos, DepthBuffer &buffer) {
        geom::Vertex vertices[3];
        triangle.getVertex(vertices, mesh);

        maths::Vector4 weight = calcTriangleWeight(vertices, scrPos);
        if (weight.w < 0)
            return;

        double z = perspCorrectionLerpZ(vertices, weight);
        buffer.writeDepthBuffer(scrPos.x, scrPos.y, z);
    }

    /**
     * 渲染正交投影的深度
     * @param triangle
     * @param mesh
     * @param scrPos
     * @param buffer
     */
    void renderOrthoPixelDepth(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                               const ScreenBuffer &buffer) {
        geom::Vertex vertices[3];
        triangle.getVertex(vertices, mesh);

        maths::Vector4 weight = calcTriangleWeight(vertices, scrPos);
        if (weight.w < 0)
            return;

        auto z = 1 - static_cast<float>(perspCorrectionLerpZ(vertices, weight));
        Color pixCol(z, z, z);

        // 四舍五入
        int x = (int) std::round(scrPos.x + 0.5);
        int y = (int) std::round(scrPos.y + 0.5);


        buffer.writeColorBuffer(x, y, pixCol.toSRGBColor().sRGBClamp(), z);
    }

    /**
     * 逐像素插值顶点颜色
     * 如果像素位置在三角形外，就不渲染
     * @param triangle
     * @param mesh
     * @param scrPos
     * @param invVPMatr
     * @param lightVPMatr
     * @param scrBuffer
     * @param depthBuffer
     */
    void renderPixelColor(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                          const maths::Matrix4x4 &invVPMatr, const maths::Matrix4x4 &lightVPMatr,
                          const ScreenBuffer &scrBuffer, const DepthBuffer &depthBuffer) {
        geom::Vertex vertices[3];
        triangle.getVertex(vertices, mesh);

        maths::Vector4 weight = calcTriangleWeight(vertices, scrPos);
        // 判断点是否在面内
        if (weight.w < 0)
            return;

        double z = perspCorrectionLerpZ(vertices, weight);

        // 将屏幕空间坐标变换到世界空间
        maths::Vector4 worldPos = screenPosToWorld(scrPos.toVector3(z), invVPMatr, scrBuffer.getWidth(), scrBuffer.getHeight(), 0.5, 100);

        // 变换到光源裁剪空间
        maths::Vector4 lightSpacePos = maths::Matrix4x4::multiply(lightVPMatr, worldPos);
        lightSpacePos = perspectiveDivision(lightSpacePos); // NDC [-1, 1]

        double shadowFactor = 1;

        if (lightSpacePos.x >= -1 && lightSpacePos.x <= 1 &&
            lightSpacePos.y >= -1 && lightSpacePos.y <= 1 &&
            lightSpacePos.z >= -1 && lightSpacePos.z <= 1 &&
            lightSpacePos.w > 0) {
            // 转换到纹理坐标 [0,1]

            lightSpacePos = viewportTransformation(lightSpacePos, depthBuffer.getWidth(), depthBuffer.getHeight());

            double lightDepth = depthBuffer.getDepth(lightSpacePos.x, lightSpacePos.y);

            if (lightDepth + 0.0005 < lightSpacePos.z)
                shadowFactor = 0.5;
        }

        Color pixCol = lerpTriangleColor(triangle, mesh, scrPos, weight, z).toSRGBColor().sRGBClamp();

        int x = (int) std::round(scrPos.x + 0.5);
        int y = (int) std::round(scrPos.y + 0.5);

        pixCol = pixCol * shadowFactor;

        scrBuffer.writeColorBuffer(x, y, pixCol, z);
    }

    /**
     * 对像素插值顶点法线，并且映射到颜色
     * @param triangle
     * @param mesh
     * @param scrPos
     * @param buffer
     */
    void renderVertexNormal(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                            const ScreenBuffer &buffer) {
        geom::Vertex vertices[3];
        maths::Vector3 normals[3];
        triangle.getVertNormals(normals, mesh);
        triangle.getVertex(vertices, mesh);

        maths::Vector4 weight = calcTriangleWeight(vertices, scrPos);
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
