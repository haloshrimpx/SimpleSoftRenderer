//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_SHADER_RENDER_H
#define SOFTRENDERER_SHADER_RENDER_H
#include "Color.h"
#include "Vertex.h"
class DirectionalLight;
class Object;

namespace shader {
    /**
    * 光栅化物体网格
    * @param object
    */
    void rasterizeObject(const Object &object);

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
     * 插值顶点颜色
     * 如果p不在三角形内，返回颜色的a将为1
     * @param triangle
     * @param p
     * @return
     */
    Color lerpTriangleColor(const geometry::Vertex triangle[3], const maths::Vector2 &p);

    void renderPixelDepth(geometry::Vertex vertices[3], const maths::Vector2 &p);

    void renderPixelColor(geometry::Vertex vertices[3], const maths::Vector2 &p);

    void renderVertexNormal(geometry::Vertex vertices[3], const maths::Vector2 &p);

    void renderTriangleWireframe(geometry::Vertex vertices[3]);
}
#endif //SOFTRENDERER_SHADER_RENDER_H
