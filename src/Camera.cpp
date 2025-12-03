//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Camera.h"

Camera::Camera(const double fov, const double zNear, const double zFar,
               const double windowWidth, const double windowHeight,
               const Transform &transform) {
    this->fov = fov;
    zoomFar = zFar;
    zoomNear = zNear;
    // 宽度与高度的比例
    aspectRatio = windowWidth / windowHeight;
    this->transform = transform;
}