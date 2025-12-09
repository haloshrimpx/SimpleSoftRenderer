//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_CAMERA_H
#define SOFTRENDERER_CAMERA_H
#include "Transform.h"

/**
    * 摄像机配置
    */
class Camera {
public:
    double fov; // 视场角
    double zoomNear; // 近裁剪平面
    double zoomFar; // 远裁剪平面
    int windowWidth; // 窗口宽度
    int windowHeight; // 窗口高度
    double aspectRatio; // 纵横比
    Transform transform; // 世界空间位置，相机的-z为视场的前方

    Camera(double fov, double zNear, double zFar,
           int windowWidth, int windowHeight,
           const Transform &transform);

    ~Camera() = default;
};
#endif //SOFTRENDERER_CAMERA_H
