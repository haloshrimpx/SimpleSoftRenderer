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
        maths::Matrix4x4 modelMatr = shader::getModelMatrix(object.transform);
        std::cout << "model matrix:" << std::endl;
        modelMatr.print();
        size_t size = object.mesh.vertices.size();

        for (int i = 0; i < size; ++i) {
            maths::Vector4 &pos = object.mesh.vertices[i].pos;
            pos = maths::Matrix4x4::multiply(modelMatr, pos);
            pos.print();
        }
    }

    void transformObjToViewSpace(Object &object, const Camera &cam) {
        maths::Matrix4x4 viewMatr = shader::getViewMatrix(cam.transform);
        size_t size = object.mesh.vertices.size();

        for (int i = 0; i < size; ++i) {
            maths::Vector4 &pos = object.mesh.vertices[i].pos;
            pos = maths::Matrix4x4::multiply(viewMatr, pos);
        }
    }

    void transformObjToPerspProjSpace(Object &object, const Camera &camera) {
        maths::Matrix4x4 perspMatr = shader::getPerspProjMatrix(camera);
        size_t size = object.mesh.vertices.size();

        for (int i = 0; i < size; ++i) {
            maths::Vector4 &pos = object.mesh.vertices[i].pos;
            pos = maths::Matrix4x4::multiply(perspMatr, pos);
        }
    }


    void backFaceCulling(Object &object) {
        for (int i = object.mesh.triangles.size() - 1; i >= 0; --i) {
            geom::Triangle &triangle = object.mesh.triangles[i];
            std::vector<geom::Vertex> vertices;
            triangle.getVertex(vertices, object.mesh);

            // edge1 = 1 - 0, edge2 = 2 - 1s

            maths::Vector3 viewDir = (vertices[0].pos - maths::VECTOR4_ZERO).toVector3();
            maths::Vector3 normal = calcTriangleNormal(vertices);

            if (maths::Vector3::dot(normal, viewDir) > 0) {
                // 在背面，删除这个三角形
                std::clog << "culling back face" << std::endl;
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
            vertex.pos = shader::viewportTransform(vertex.pos, screenWidth, screenHeight,
                                                   0, 0);
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

    maths::Vector4 viewportTransform(const maths::Vector4 &ndcVec, const int scrW, const int scrH, const int viewportX,
                                     const int viewportY) {
        // Matrix4x4 viewportMatr = getViewportTransMatrix(scrW, scrH);
        // return Matrix4x4::multiply(viewportMatr, ndcVec);
        // y轴向上
        // x_screen = (x_ndc + 1) * (width / 2) + x
        // y_screen = (y_ndc + 1) * (height / 2) + y
        // z_screen = (z_ndc + 1) * 0.5

        double x = (ndcVec.x + 1) / 2 * scrW;

        // OpenGL的屏幕坐标系的原点在左下角，Windows控制台的坐标系原点在左上角
        // 变换到Windows控制台的形式R
        double y = (1 - ndcVec.y) / 2 * scrH;
        double z = (ndcVec.z + 1) * 0.5; // 映射到[0, 1]

        return {
            x, y, z, ndcVec.w
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
}
