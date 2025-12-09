//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_SHADER_RENDER_H
#define SOFTRENDERER_SHADER_RENDER_H
#include <memory>
#include <vector>

#include "Color.h"
#include "Vertex.h"

class PointLight;
class DirectionalLight;

namespace geom {
    class Mesh;
    class Triangle;
}

class Camera;
class ScreenBuffer;
class Light;
class Object;

namespace shader {
    /**
     * 剔除选项
     */
    enum class CullingMode {
        NONE,
        BACK,
        FRONT,
    };

    /**
     * 渲染缓冲区
     * @param buffer
     */
    void renderBuffer(const ScreenBuffer &buffer);

    /**
     * 顶点着色插值的渲染管线
     * @param objects 要被渲染的物体列表
     * @param light 主光源
     * @param camera 摄像机
     * @param buffer 目标缓冲区
     */
    void gouraudShadingPipeline(const std::vector<Object> &objects, const std::vector<std::unique_ptr<Light> > &lights,
                                const Camera &camera,
                                const ScreenBuffer &buffer);

    /**
    * 光栅化物体网格
    * @param object
    * @param buffer
    */
    void rasterizeObject(const Object &object, const ScreenBuffer &buffer);

    /**
     * 为物体的顶点计算光照
     * @param object
     * @param lights
     * @param ambient
     * @param camPos
     */
    void shadingVertex(Object &object, const std::vector<std::unique_ptr<Light> > &lights, const maths::Vector3 &camPos,
                       const Color &ambient);

    void handlePointLight(Object &object, const PointLight *pointLight, const maths::Vector3 &camPos);

    void handleDirectionalLight(Object &object, const DirectionalLight *dirLight, const maths::Vector3 &camPos);

    /**
     * 为物体的面计算光照
     * @param object
     * @param dirLight
     */
    void shadingFace(Object &object, const std::vector<std::unique_ptr<Light> > &lights);

    /**
     * 求位置对于三角形重心的权重
     * @param triangle
     * @param p
     * @return x: alpha | y: beta | z: gamma | w:flag
     */
    maths::Vector4 getTriangleWeight(const geom::Vertex triangle[3], const maths::Vector2 &p);

    /**
    * 求位置对于三角形重心的权重，预计算三角形面积
    * @param triangle
    * @param area
    * @param p
    * @return x: alpha | y: beta | z: gamma | w:flag
    */
    maths::Vector4 getTriangleWeight(const geom::Vertex triangle[3], double area, const maths::Vector2 &p);

    /**
     *
     * @param triangle 透视矫正插值
     * @param weight
     * @param iA
     * @param iB
     * @param iC
     * @return
     */
    double perspCorrectionLerp(const geom::Vertex triangle[3],
                               const maths::Vector4 &weight,
                               double iA, double iB, double iC);

    maths::Vector3 perspCorrectionLerp(const geom::Vertex triangle[3],
                                       const maths::Vector4 &weight,
                                       const maths::Vector3 &iA, const maths::Vector3 &iB, const maths::Vector3 &iC);

    maths::Vector4 perspCorrectionLerp(const geom::Vertex triangle[3],
                                       const maths::Vector4 &weight,
                                       const maths::Vector4 &iA, const maths::Vector4 &iB, const maths::Vector4 &iC);

    double perspCorrectionLerpZ(const geom::Vertex triangle[3], const maths::Vector4 &weight);

    /**
     * 插值顶点颜色
     * 如果p不在三角形内，返回颜色的a将为1
     * @param triangle
     * @param mesh
     * @param p
     * @param z
     * @return
     */
    Color lerpTriangleColor(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &p, const maths::Vector4 &weight,
                            double z);

    void renderLinearPixelDepth(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                                const ScreenBuffer &buffer, float n, float f);

    void renderPixelColor(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                          const ScreenBuffer &buffer);

    void renderVertexNormal(const geom::Triangle &triangle, const geom::Mesh &mesh, const maths::Vector2 &scrPos,
                            const ScreenBuffer &buffer);

    void renderTriangleWireframe(geom::Vertex vertices[3]);
}
#endif //SOFTRENDERER_SHADER_RENDER_H
