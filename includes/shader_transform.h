//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_SHADER_TRANSFORM_H
#define SOFTRENDERER_SHADER_TRANSFORM_H
#include "shader_render.h"
class Camera;
class Transform;
class Object;

namespace maths {
    class Vector3;
    class Vector4;
    class Matrix4x4;
}

namespace shader {
    /* 变换过程:
     * 模型空间
     * -> (模型矩阵 Model Matrix) -> 世界空间
     * -> (观察矩阵 View Matrix) -> 观察空间(相机坐标系)
     * -> (投影矩阵 Projection Matrix) -> 裁剪空间(齐次裁剪坐标系)
     * -> (透视除法 Perspective Division) -> NDC坐标系
     * -> (视口变换 Viewport Transform) -> 屏幕坐标系
     */


    /**
     * 将物体顶点组从模型空间变换到世界空间
     * @param object
     */
    void transformObjToWorldSpace(Object &object);

    /**
     * 将物体顶点组从世界空间变换到观察空间
     * @param object
     * @param camTransform
     */
    void transformObjToViewSpace(Object &object, const Transform &camTransform);

    /**
     * 将物体顶点组从观察变换到透视裁剪空间
     * @param object
     * @param camera
     */
    void transformObjToPerspProjSpace(Object &object, const Camera &camera);

    /**
     * 将物体顶点组变换到正交裁剪空间
     * @param object
     * @param camera
     */
    void transformObjToOrthoProjSpace(Object &object, const Camera &camera);

    /**
     * 在观察空间中对三角形进行正面/背面剔除
     * @param object
     * @param mode 剔除模式
     */
    void cullingFaces(Object &object, const CullingMode &mode);

    /**
     * 对物体顶点组进行透视除法，变换到NDC中
     * @param object
     */
    void applyPerspectiveDivision(Object &object);

    /**
     * 对物体顶点组进行视口变换
     * @param object
     * @param screenWidth
     * @param screenHeight
     */
    void transformObjToViewportSpace(Object &object, int screenWidth, int screenHeight);


    maths::Matrix4x4 getTranslationMatrix(double tx, double ty, double tz);

    maths::Matrix4x4 getTranslationMatrix(const maths::Vector3 &position);

    maths::Matrix4x4 getScaleMatrix(double sx, double sy, double sz);

    maths::Matrix4x4 getScaleMatrix(const maths::Vector3 &scale);

    maths::Matrix4x4 getRotationMatrix(const maths::Vector3 &rotation);

    maths::Matrix4x4 getRotationMatrixX(double degreeX);

    maths::Matrix4x4 getRotationMatrixY(double degreeY);

    maths::Matrix4x4 getRotationMatrixZ(double degreeZ);

    maths::Matrix4x4 getRightToLeftCoordMatrix();

    /**
     * 获取将坐标从模型空间变换到世界空间的矩阵
     * @param objWorldTransform 模型空间的变换
     * @return 模型矩阵
     */
    maths::Matrix4x4 getModelMatrix(const Transform &objWorldTransform);

    /**
     * 获取将坐标从世界空间变换到观察空间
     * @param worldCamTransform 世界空间的相机坐标
     * @return 观察矩阵
     */
    maths::Matrix4x4 getViewMatrix(const Transform &worldCamTransform);

    /**
     * 获取将坐标从观察空间变换到透视投影空间的矩阵
     * @param cam 相机配置
     * @return 透视投影矩阵
     */
    maths::Matrix4x4 getPerspProjMatrix(const Camera &cam);

    maths::Matrix4x4 getPerspMVPMatrix(const Camera &cam, const Transform &worldCamTransform);

    maths::Matrix4x4 getPerspVPMatrix(const Camera &cam, const Transform &worldTransform);

    maths::Matrix4x4 getOrthoMVPMatrix(const Camera &cam, const Transform &worldCamTransform);

    /**
     * 获取正交投影矩阵
     * @param aspect 宽高比
     * @param orthoSize 视景体高度的一半, 相当于缩放因子, 越大看到的东西越多
     * @param near 近平面
     * @param far 远平面
     * @return
     */
    maths::Matrix4x4 getOrthoProjMatrix(double aspect, double orthoSize, double near, double far);

    /**
     * 透视除法
     * @param projSpaceVec 裁剪空间内的点
     * @return NDC坐标空间的点，w分量变为1
     */
    maths::Vector4 perspectiveDivision(const maths::Vector4 &projSpaceVec);

    /**
     * 视口变换
     * @param ndcVec NDC空间中的顶点坐标
     * @param scrW 窗口宽度
     * @param scrH 窗口高度
     * @return
     */
    maths::Vector4 viewportTransformation(const maths::Vector4 &ndcVec, int scrW, int scrH);

    maths::Vector4 translation(const maths::Vector4 &vec, double x, double y, double z);

    maths::Vector4 translation(const maths::Vector4 &vec, const maths::Vector3 &delta);

    maths::Vector4 scale(const maths::Vector4 &vec, double x, double y, double z);

    maths::Vector4 scale(const maths::Vector4 &vec, const maths::Vector3 &k);

    maths::Vector4 rotateWorld(const maths::Vector4 &vec, const maths::Vector3 &deltaDegree);

    maths::Vector4 rotateWorld(const maths::Vector4 &vec, double degX, double degY, double degZ);

    maths::Vector4 rotateLocal(const maths::Vector4 &vec, const maths::Vector3 &deltaDegree);

    maths::Vector4 rotateLocal(const maths::Vector4 &vec, double degX, double degY, double degZ);

    maths::Vector4 screenPosToWorld(const maths::Vector3 &screenPos, const maths::Matrix4x4 &invVPMatrix,
                                    const int scrW, const int scrH, const double near, const double far);

    double ndcToLinear01Depth(double ndcDepth, double near, double far);
}

#endif //SOFTRENDERER_SHADER_TRANSFORM_H
