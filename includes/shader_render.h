//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_SHADER_RENDER_H
#define SOFTRENDERER_SHADER_RENDER_H
#include "Color.h"
#include "Vertex.h"
class Camera;
class Buffer;
class DirectionalLight;
class Object;

namespace shader {
    /**
     * 渲染缓冲区
     * @param buffer
     */
    void renderBuffer(const Buffer &buffer);

    /**
     * 顶点着色插值的渲染管线
     * @param renderObject 目标物体
     * @param light 主光源
     * @param camera 摄像机
     * @param buffer 目标缓冲区
     * @param screenWidth 屏幕高度
     * @param screenHeight 屏幕宽度
     */
    void vertexShadingPipeline(const Object &renderObject, const DirectionalLight &light, const Camera &camera,
                               const Buffer &buffer, int screenWidth, int screenHeight);

    /**
    * 光栅化物体网格
    * @param object
    * @param buffer
    */
    void rasterizeObject(const Object &object, const Buffer &buffer);

    /**
     * 为物体的顶点计算光照
     * @param object
     * @param dirLight
     * @param ambient
     * @param camPos
     */

    void shadingVertex(Object &object, const DirectionalLight &dirLight, const maths::Vector3 &camPos,
                       const Color &ambient);

    /**
     * 为物体的面计算光照
     * @param object
     * @param dirLight
     */
    void shadingFace(Object &object, const DirectionalLight &dirLight);

    /**
     * 求位置对于三角形重心的权重
     * @param triangle
     * @param p
     * @return x: alpha | y: beta | z: gamma | w:flag
     */
    maths::Vector4 getTriangleWeight(const geometry::Vertex triangle[3], const maths::Vector2 &p);

    /**
    * 求位置对于三角形重心的权重，预计算三角形面积
    * @param triangle
    * @param area
    * @param p
    * @return x: alpha | y: beta | z: gamma | w:flag
    */
    maths::Vector4 getTriangleWeight(const geometry::Vertex triangle[3], double area, const maths::Vector2 &p);

    /**
     *
     * @param triangle 透视矫正插值
     * @param weight
     * @param iA
     * @param iB
     * @param iC
     * @return
     */
    double perspCorrectionLerp(const geometry::Vertex triangle[3],
                               const maths::Vector4 &weight,
                               const double iA, const double iB, const double iC);

    maths::Vector3 perspCorrectionLerp(const geometry::Vertex triangle[3],
                                       const maths::Vector4 &weight,
                                       const maths::Vector3 &iA, const maths::Vector3 &iB, const maths::Vector3 &iC);

    maths::Vector4 perspCorrectionLerp(const geometry::Vertex triangle[3],
                                       const maths::Vector4 &weight,
                                       const maths::Vector4 &iA, const maths::Vector4 &iB, const maths::Vector4 &iC);

    /**
     * 插值顶点颜色
     * 如果p不在三角形内，返回颜色的a将为1
     * @param triangle
     * @param p
     * @return
     */
    Color lerpTriangleColor(const geometry::Vertex triangle[3], const maths::Vector2 &p);

    Color lerpTriangleColor(const geometry::Vertex triangle[3], const maths::Vector2 &p, const maths::Vector4 &weight,
                            double z);

    void renderPixelDepth(geometry::Vertex vertices[3], const maths::Vector2 &p, const Buffer &buffer);

    void renderPixelColor(geometry::Vertex vertices[3], const maths::Vector2 &p, const Buffer &buffer);

    void renderVertexNormal(geometry::Vertex vertices[3], const maths::Vector2 &p, const Buffer &buffer);

    void renderTriangleWireframe(geometry::Vertex vertices[3]);
}
#endif //SOFTRENDERER_SHADER_RENDER_H
