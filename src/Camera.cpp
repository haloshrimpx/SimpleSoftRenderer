//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Camera.h"

Camera::Camera(const double fov, const double zNear, const double zFar,
               const int windowWidth, const int windowHeight,
               const Transform &transform) {
    this->fov = fov;
    zoomFar = zFar;
    zoomNear = zNear;
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
    // 宽度与高度的比例
    aspectRatio = (double) windowWidth / windowHeight;
    this->transform = transform;
}
