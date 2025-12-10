//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/shader_transform.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <ostream>

#include "../includes/Matrix.h"
#include "../includes/Object.h"
#include "../includes/Mesh.h"
#include "../includes/Vertex.h"
#include "../includes/Camera.h"
#include "../includes/shader_geometry.h"

namespace shader {
    void transformObjToWorldSpace(Object &object) {
        // 变换顶点
        maths::Matrix4x4 modelMatr = getModelMatrix(object.transform);
        std::cout << "model matrix:" << std::endl;
        modelMatr.print();
        size_t size = object.mesh.vertices.size();

        for (int i = 0; i < size; ++i) {
            maths::Vector4 &pos = object.mesh.vertices[i].pos;
            pos = maths::Matrix4x4::multiply(modelMatr, pos);
        }

        // 变换法线
        // NormalMatrix 是 ModelMatrix 的逆矩阵的转置矩阵( (M^-1)^T )
        maths::Matrix4x4 normalMatr = modelMatr.inverse().transpose();
        size = object.mesh.normals.size();

        for (int i = 0; i < size; ++i) {
            maths::Vector3 &normal = object.mesh.normals[i];
            normal = maths::Matrix4x4::multiply(normalMatr, normal.toVector4(0)).toVector3().normalize();
        }
    }

    void transformObjToViewSpace(Object &object, const Transform &camTransform) {
        maths::Matrix4x4 viewMatr = getViewMatrix(camTransform);
        size_t size = object.mesh.vertices.size();

        for (int i = 0; i < size; ++i) {
            maths::Vector4 &pos = object.mesh.vertices[i].pos;
            pos = maths::Matrix4x4::multiply(viewMatr, pos);
        }
    }

    void transformObjToPerspProjSpace(Object &object, const Camera &camera) {
        maths::Matrix4x4 perspMatr = getPerspProjMatrix(camera);
        //  getOrthoProjMatrix(camera.aspectRatio, 2, camera.zoomNear, camera.zoomFar);

        size_t size = object.mesh.vertices.size();

        for (int i = 0; i < size; ++i) {
            maths::Vector4 &pos = object.mesh.vertices[i].pos;
            pos = maths::Matrix4x4::multiply(perspMatr, pos);
        }
    }

    void transformObjToOrthoProjSpace(Object &object, const Camera &camera) {
        maths::Matrix4x4 perspMatr = getOrthoProjMatrix(camera.aspectRatio, 2, camera.zoomNear, camera.zoomFar);

        size_t size = object.mesh.vertices.size();

        for (int i = 0; i < size; ++i) {
            maths::Vector4 &pos = object.mesh.vertices[i].pos;
            pos = maths::Matrix4x4::multiply(perspMatr, pos);
        }
    }

    void cullingFaces(Object &object, const CullingMode &mode) {
        for (int i = object.mesh.triangles.size() - 1; i >= 0; --i) {
            geom::Triangle &triangle = object.mesh.triangles[i];
            std::vector<geom::Vertex> vertices;
            triangle.getVertex(vertices, object.mesh);

            // edge1 = 1 - 0, edge2 = 2 - 1s

            maths::Vector3 viewDir = (vertices[0].pos - maths::VECTOR4_ZERO).toVector3();
            maths::Vector3 normal = calcTriangleNormal(vertices);

            // 检查剔除选项
            bool flag = false;
            switch (mode) {
                case CullingMode::BACK:
                    flag = maths::Vector3::dot(normal, viewDir) > 0;
                    break;
                case CullingMode::FRONT:
                    flag = maths::Vector3::dot(normal, viewDir) < 0;
                    break;
                case CullingMode::NONE:
                    return;
            }

            if (flag) {
                object.mesh.triangles.erase(object.mesh.triangles.begin() + i);
            }
        }
    }

    void applyPerspectiveDivision(Object &object) {
        for (auto &vertice: object.mesh.vertices) {
            maths::Vector4 &pos = vertice.pos;
            pos = shader::perspectiveDivision(pos);
        }
    }

    void transformObjToViewportSpace(Object &object, int screenWidth, int screenHeight) {
        for (auto &vertex: object.mesh.vertices) {
            vertex.pos = viewportTransformation(vertex.pos, screenWidth, screenHeight);
        }
    }

    maths::Matrix4x4 getTranslationMatrix(double tx, double ty, double tz) {
        double matr[4][4] = {
            {1, 0, 0, tx},
            {0, 1, 0, ty},
            {0, 0, 1, tz},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }

    maths::Matrix4x4 getTranslationMatrix(const maths::Vector3 &position) {
        double matr[4][4] = {
            {1, 0, 0, position.x},
            {0, 1, 0, position.y},
            {0, 0, 1, position.z},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }

    maths::Matrix4x4 getScaleMatrix(double sx, double sy, double sz) {
        double matr[4][4] = {
            {sx, 0, 0, 0},
            {0, sy, 0, 0},
            {0, 0, sz, 0},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }

    maths::Matrix4x4 getScaleMatrix(const maths::Vector3 &scale) {
        double matr[4][4] = {
            {scale.x, 0, 0, 0},
            {0, scale.y, 0, 0},
            {0, 0, scale.z, 0},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }

    /**
     * 获取旋转矩阵，ZYX 顺序
     * @param rotation
     * @return
     */
    maths::Matrix4x4 getRotationMatrix(const maths::Vector3 &rotation) {
        maths::Matrix4x4 xRotationMatrix = getRotationMatrixX(rotation.x);
        maths::Matrix4x4 yRotationMatrix = getRotationMatrixY(rotation.y);
        maths::Matrix4x4 zRotationMatrix = getRotationMatrixZ(rotation.z);

        return maths::Matrix4x4::multiply(xRotationMatrix,
                                          maths::Matrix4x4::multiply(yRotationMatrix, zRotationMatrix));
    }

    maths::Matrix4x4 getRotationMatrixX(double degreeX) {
        double rad = degreeX * std::numbers::pi / 180;
        double matr[4][4] = {
            {1, 0, 0, 0},
            {0, std::cos(rad), -std::sin(rad), 0},
            {0, std::sin(rad), std::cos(rad), 0},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }

    maths::Matrix4x4 getRotationMatrixY(double degreeY) {
        double rad = degreeY * std::numbers::pi / 180;
        double matr[4][4] = {
            {std::cos(rad), 0, std::sin(rad), 0},
            {0, 1, 0, 0},
            {-std::sin(rad), 0, std::cos(rad), 0},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }

    maths::Matrix4x4 getRotationMatrixZ(double degreeZ) {
        double rad = degreeZ * std::numbers::pi / 180;
        double matr[4][4] = {
            {std::cos(rad), -std::sin(rad), 0, 0},
            {std::sin(rad), std::cos(rad), 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }

    maths::Matrix4x4 getRightToLeftCoordMatrix() {
        double matr[4][4] = {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, -1, 0},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }

    maths::Matrix4x4 getModelMatrix(const Transform &objWorldTransform) {
        // 模型矩阵 = 平移矩阵 × 旋转矩阵 × 缩放矩阵
        // 按 ZYX 顺序构建旋转矩阵

        maths::Matrix4x4 scaleMatrix = getScaleMatrix(objWorldTransform.getScale());
        maths::Matrix4x4 translationMatrix = getTranslationMatrix(objWorldTransform.getPosition());

        const maths::Vector3 &objRotation = objWorldTransform.getRotation();
        maths::Matrix4x4 xRotationMatrix = getRotationMatrixX(objRotation.x);
        maths::Matrix4x4 yRotationMatrix = getRotationMatrixY(objRotation.y);
        maths::Matrix4x4 zRotationMatrix = getRotationMatrixZ(objRotation.z);

        maths::Matrix4x4 rotationMatrix = maths::Matrix4x4::multiply(xRotationMatrix,
                                                                     maths::Matrix4x4::multiply(
                                                                         yRotationMatrix, zRotationMatrix));

        return maths::Matrix4x4::multiply(translationMatrix, maths::Matrix4x4::multiply(rotationMatrix, scaleMatrix));
    }

    maths::Matrix4x4 getViewMatrix(const Transform &worldCamTransform) {
        // M view= M rotation × M translation
        // M view =M negate * M view 取反
        // 相机的缩放无意义
        // https://zhuanlan.zhihu.com/p/552252893

        const maths::Vector3 &camRotation = worldCamTransform.getRotation();
        maths::Matrix4x4 xRotationMatrix = getRotationMatrixX(camRotation.x);
        maths::Matrix4x4 yRotationMatrix = getRotationMatrixY(camRotation.y);
        maths::Matrix4x4 zRotationMatrix = getRotationMatrixZ(camRotation.z);

        // 平移矩阵 是 相机世界平移矩阵 的 逆矩阵
        maths::Matrix4x4 translationMatrix = getTranslationMatrix(worldCamTransform.getPosition()).inverse();

        // 旋转矩阵 是 相机世界空间 旋转矩阵 的 转置矩阵
        maths::Matrix4x4 rotationMatrix = maths::Matrix4x4::multiply(xRotationMatrix,
                                                                     maths::Matrix4x4::multiply(
                                                                         yRotationMatrix, zRotationMatrix)).
                transpose();
        return maths::Matrix4x4::multiply(rotationMatrix, translationMatrix);
    }

    maths::Matrix4x4 getPerspProjMatrix(const Camera &cam) {
        const double camFovRad = (cam.fov / 2) * M_PI / 180; // 转换为弧度制

        const double Fy = 1 / tan(camFovRad); // 垂直方向的缩放 : cot(fov / 2)
        const double Fx = Fy / cam.aspectRatio; // 水平方向的缩放

        const double B = -((cam.zoomNear + cam.zoomFar) / (cam.zoomFar - cam.zoomNear));
        const double C = -(2 * cam.zoomNear * cam.zoomFar / (cam.zoomFar - cam.zoomNear));

        double matr[4][4] = {
            {Fx, 0, 0, 0},
            {0, Fy, 0, 0},
            {0, 0, B, C},
            {0, 0, -1, 0}
        };
        return maths::Matrix4x4(matr);
    }

    /**
     * 获取透视投影的MVP矩阵
     * @param cam
     * @param worldCamTransform
     * @return
     */
    maths::Matrix4x4 getPerspMVPMatrix(const Camera &cam, const Transform &worldCamTransform) {
        maths::Matrix4x4 modelMatr = getModelMatrix(worldCamTransform);
        maths::Matrix4x4 viewMatr = getViewMatrix(worldCamTransform);
        maths::Matrix4x4 perspMatr = getPerspProjMatrix(cam);

        return maths::Matrix4x4::multiply(perspMatr, maths::Matrix4x4::multiply(viewMatr, modelMatr));
    }

    maths::Matrix4x4 getPerspVPMatrix(const Camera &cam, const Transform &worldTransform) {
        maths::Matrix4x4 viewMatr = getViewMatrix(worldTransform);
        maths::Matrix4x4 perspMatr = getPerspProjMatrix(cam);

        return maths::Matrix4x4::multiply(perspMatr, viewMatr);
    }

    maths::Matrix4x4 getOrthoVPMatrix(const Camera &cam, const Transform &worldTransform) {
        maths::Matrix4x4 viewMatr = getViewMatrix(worldTransform);
        maths::Matrix4x4 perspMatr = getPerspProjMatrix(cam);

        return maths::Matrix4x4::multiply(perspMatr, viewMatr);
    }

    /**
     * 获取正交投影的MVP矩阵
     * @param cam
     * @param worldCamTransform
     * @return
     */
    maths::Matrix4x4 getOrthoMVPMatrix(const Camera &cam, const Transform &worldCamTransform) {
        maths::Matrix4x4 modelMatr = getModelMatrix(worldCamTransform);
        maths::Matrix4x4 viewMatr = getViewMatrix(worldCamTransform);
        maths::Matrix4x4 orthoMatr = getOrthoProjMatrix(cam.aspectRatio, 2, cam.zoomNear, cam.zoomFar);

        return maths::Matrix4x4::multiply(orthoMatr, maths::Matrix4x4::multiply(viewMatr, modelMatr));
    }

    maths::Matrix4x4 getOrthoProjMatrix(double aspect, double orthoSize, double near, double far) {
        /*
         * [ 2/(r-l),     0,         0,        -(r+l)/(r-l) ]
         * [    0,     2/(t-b),      0,        -(t+b)/(t-b) ]
         * [    0,         0,     -2/(f-n),    -(f+n)/(f-n) ]
         * [    0,         0,         0,            1       ]
         */

        double right = orthoSize * aspect;
        double left = -orthoSize * aspect;
        double top = orthoSize;
        double bottom = -orthoSize;

        double matr[4][4] = {
            {2 / (right - left), 0, 0, -(right + left) / (right - left)},
            {0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom)},
            {0, 0, -2 / (far - near), -(far + near) / (far - near)},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }


    maths::Matrix4x4 getViewportTransMatrix(const double w, const double h) {
        /* M_viewport
            [width/2,       0,          0,      x + width/2],
            [0,       height/2,        0,      y + height/2],
            [0,             0,          d/2,    d/2        ],
            [0,             0,          0,          1      ]
        */
        double d = 1;
        double matr[4][4] = {
            {w / 2, 0, 0, w / 2},
            {0, h / 2, 0, h / 2},
            {0, 0, d / 2, d / 2},
            {0, 0, 0, 1}
        };
        return maths::Matrix4x4(matr);
    }

    maths::Vector4 perspectiveDivision(const maths::Vector4 &projSpaceVec) {
        return {
            projSpaceVec.x / projSpaceVec.w,
            projSpaceVec.y / projSpaceVec.w,
            projSpaceVec.z / projSpaceVec.w,
            1
        };
    }

    maths::Vector4 viewportTransformation(const maths::Vector4 &ndcVec, const int scrW, const int scrH) {
        double x = (ndcVec.x + 1) / 2 * scrW;
        // OpenGL的屏幕坐标系的原点在左下角，Windows控制台的坐标系原点在左上角
        // 变换到Windows控制台的形式R
        double y = (1 - ndcVec.y) / 2 * scrH;
        double z = ndcVec.z * 0.5 + 0.5; // 映射到[0, 1]

        return {
            x, y, z, 1
        };
    }

    maths::Vector4 translation(const maths::Vector4 &vec, double x, double y, double z) {
        maths::Matrix4x4 translationMatrix = getTranslationMatrix(x, y, z);
        return maths::Matrix4x4::multiply(translationMatrix, vec);
    }

    maths::Vector4 translation(const maths::Vector4 &vec, const maths::Vector3 &delta) {
        return translation(vec, delta.x, delta.y, delta.z);
    }

    maths::Vector4 scale(const maths::Vector4 &vec, double x, double y, double z) {
        maths::Matrix4x4 scaleMatrix = getScaleMatrix(x, y, z);
        return maths::Matrix4x4::multiply(scaleMatrix, vec);
    }

    maths::Vector4 scale(const maths::Vector4 &vec, const maths::Vector3 &k) {
        return scale(vec, k.x, k.y, k.z);
    }

    maths::Vector4 rotateWorld(const maths::Vector4 &vec, const maths::Vector3 &deltaDegree) {
        // 对于世界空间旋转（绕固定轴），公式为 Z ( Y ( X v ) )
        double xDeg = deltaDegree.x;
        double yDeg = deltaDegree.y;
        double zDeg = deltaDegree.z;

        maths::Matrix4x4 xRotationMatrix = getRotationMatrixX(xDeg);
        maths::Matrix4x4 yRotationMatrix = getRotationMatrixY(yDeg);
        maths::Matrix4x4 zRotationMatrix = getRotationMatrixZ(zDeg);

        return maths::Matrix4x4::multiply(zRotationMatrix,
                                          maths::Matrix4x4::multiply(yRotationMatrix,
                                                                     maths::Matrix4x4::multiply(xRotationMatrix, vec)));
    }

    maths::Vector4 rotateWorld(const maths::Vector4 &vec, double degX, double degY, double degZ) {
        maths::Matrix4x4 xRotationMatrix = getRotationMatrixX(degX);
        maths::Matrix4x4 yRotationMatrix = getRotationMatrixY(degY);
        maths::Matrix4x4 zRotationMatrix = getRotationMatrixZ(degZ);

        return maths::Matrix4x4::multiply(zRotationMatrix,
                                          maths::Matrix4x4::multiply(yRotationMatrix,
                                                                     maths::Matrix4x4::multiply(xRotationMatrix, vec)));
    }

    maths::Vector4 rotateLocal(const maths::Vector4 &vec, const maths::Vector3 &deltaDegree) {
        // 对于世界空间旋转（绕固定轴），公式为 X(Y(Z(v)))
        double xDeg = deltaDegree.x;
        double yDeg = deltaDegree.y;
        double zDeg = deltaDegree.z;

        maths::Matrix4x4 xRotationMatrix = getRotationMatrixX(xDeg);
        maths::Matrix4x4 yRotationMatrix = getRotationMatrixY(yDeg);
        maths::Matrix4x4 zRotationMatrix = getRotationMatrixZ(zDeg);

        return maths::Matrix4x4::multiply(xRotationMatrix,
                                          maths::Matrix4x4::multiply(yRotationMatrix,
                                                                     maths::Matrix4x4::multiply(zRotationMatrix, vec)));
    }

    maths::Vector4 rotateLocal(const maths::Vector4 &vec, double degX, double degY, double degZ) {
        maths::Matrix4x4 xRotationMatrix = getRotationMatrixX(degX);
        maths::Matrix4x4 yRotationMatrix = getRotationMatrixY(degY);
        maths::Matrix4x4 zRotationMatrix = getRotationMatrixZ(degZ);

        return maths::Matrix4x4::multiply(xRotationMatrix,
                                          maths::Matrix4x4::multiply(yRotationMatrix,
                                                                     maths::Matrix4x4::multiply(zRotationMatrix, vec)));
    }

    /**
     * 将屏幕坐标转换为世界坐标
     * @param screenPos
     * @param invVPMatrix VP矩阵的逆矩阵
     * @param scrW
     * @param scrH
     * @param near
     * @param far
     * @return
     */
    maths::Vector4 screenPosToWorld(const maths::Vector3 &screenPos, const maths::Matrix4x4 &invVPMatrix,
                                    const int scrW, const int scrH, const double near, const double far) {
        // 转换为NDC坐标
        double ndcX = 2 * screenPos.x / scrW - 1;
        double ndcY = 1 - 2 * screenPos.y / scrH; // Y反转
        double ndcZ = 2 * screenPos.z - 1;

        // 方法：重建正确 w_c 的裁剪坐标
        // 获取投影矩阵参数
        double n = near;
        double f = far;
        double A = -(f + n) / (f - n);
        double B = -2 * f * n / (f - n);

        // 从 ndcZ 反算视图空间 z_v
        double z_v = B / (-ndcZ - A);
        double w = -z_v;

        // 裁剪空间坐标
        maths::Vector4 pos = {ndcX * w, ndcY * w, ndcZ * w, w};

        pos = maths::Matrix4x4::multiply(invVPMatrix, pos); // 逆变换 到 世界坐标

        // 透视除法
        pos *= 1 / pos.w;

        // 返回世界坐标
        return pos;
    }

    /**
     * 将NDC[-1, 1]的深度转换为线性深度
     * @param ndcDepth
     * @param near
     * @param far
     * @return
     */
    double ndcToLinear01Depth(double ndcDepth, double near, double far) {
        return (2.0 * near * far) / (far + near - ndcDepth * (far - near));
    }
}
