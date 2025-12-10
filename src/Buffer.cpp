//
// Created by Haloshrimp on 2025/12/4.
//

#include "../includes/Buffer.h"

#include <algorithm>
#include <iostream>

Buffer::Buffer(int width, int height) {
    bufferWidth = width;
    bufferHeight = height;
}

int Buffer::getBufferSize() const {
    return bufferWidth * bufferHeight;
}

int Buffer::getHeight() const {
    return bufferHeight;
}

int Buffer::getWidth() const {
    return bufferWidth;
}

int Buffer::getIndex(int x, int y) const {
    return y * bufferWidth + x;
}

/**
 * 初始化缓冲区
 * @param width 屏幕宽度
 * @param height 屏幕高度
 */
ScreenBuffer::ScreenBuffer(const int width, const int height) : Buffer(width, height) {
    int size = width * height;

    colorBuffer = new Color[size]{};
    zBuffer = new double[size]{};
}

/**
 * 析构缓冲区
 */
ScreenBuffer::~ScreenBuffer() {
    delete[] colorBuffer;
    delete[] zBuffer;
}

/**
 * 获取深度缓冲区的只读指针
 * @return
 */
const double *ScreenBuffer::getZBuffer() const {
    return zBuffer;
}

/**
 * 获取颜色缓冲区的只读指针
 * @return
 */
const Color *ScreenBuffer::getColorBuffer() const {
    return colorBuffer;
}

/**
 * 向颜色缓冲区写入颜色值
 * @param x 坐标x
 * @param y 坐标y
 * @param sRGBCol 8位颜色值 [0, 255]
 * @param z 坐标的深度
 * @return 写入是否成功
 */
bool ScreenBuffer::writeColorBuffer(int x, int y, const Color &sRGBCol, double z) const {
    int idx = getIndex(x, y);
    int maxIdx = getIndex(getWidth() - 1, getHeight() - 1);

    // 检查索引是否越界
    if (idx > maxIdx || idx < 0)
        return false;

    double bufferZ = zBuffer[idx];

    // z Near [0 - 1] Far
    // 离屏幕越近深度越小，写入
    if (z < bufferZ) {
        colorBuffer[idx] = sRGBCol; // 更新颜色
        zBuffer[idx] = z; // 更新深度
        return true;
    }
    // std::cout << "z write failed: " << z << std::endl;
    return false;
}

/**
 * 清空缓冲区
 */
void ScreenBuffer::clear() const {
    std::fill_n(colorBuffer, getBufferSize(), Color{0, 0, 0, 0});
    std::fill_n(zBuffer, getBufferSize(), 1);
}

DepthBuffer::DepthBuffer(int width, int height) : Buffer(width, height) {
    int size = width * height;

    depthBuffer = new double[size]{};
    this->clear();
}

DepthBuffer::~DepthBuffer() {
    delete [] depthBuffer;
}

bool DepthBuffer::writeDepthBuffer(int x, int y, double z) const {
    int idx = getIndex(x, y);
    int maxIdx = getIndex(getWidth() - 1, getHeight() - 1);

    // 检查索引是否越界
    if (idx > maxIdx || idx < 0) return false;

    double bufferZ = depthBuffer[idx];

    // z Near [0 - 1] Far
    // 离屏幕越近深度越小，写入
    if (z < bufferZ) {
        depthBuffer[idx] = z; // 更新深度

        return true;
    }

    return false;
}

double DepthBuffer::getDepth(int x, int y) const {
    int idx = getIndex(x, y);
    int maxIdx = getIndex(getWidth() - 1, getHeight() - 1);

    // 检查索引是否越界
    if (idx > maxIdx || idx < 0)
        return 1;

    return depthBuffer[idx];
}

void DepthBuffer::clear() const {
    std::fill_n(depthBuffer, getBufferSize(), 1);
}
