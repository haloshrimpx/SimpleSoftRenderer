//
// Created by Haloshrimp on 2025/12/4.
//

#include "Buffer.h"

#include <iostream>

/**
 * 初始化缓冲区
 * @param width 屏幕宽度
 * @param height 屏幕高度
 */
Buffer::Buffer(int width, int height) {
    int size = width * height;
    bufferHeight = height;
    bufferWidth = width;
    colorBuffer = new Color[size];
    zBuffer = new double[size]{};
}

/**
 * 析构缓冲区
 */
Buffer::~Buffer() {
    delete[] colorBuffer;
    delete[] zBuffer;
}

maths::Vector2 Buffer::getBufferSize() const {
    return {static_cast<double>(bufferWidth), static_cast<double>(bufferHeight)};
}

/**
 * 获取深度缓冲区的只读指针
 * @return
 */
const double *Buffer::getZBuffer() const {
    return zBuffer;
}

/**
 * 获取颜色缓冲区的只读指针
 * @return
 */
const Color *Buffer::getColorBuffer() const {
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
bool Buffer::writeColorBuffer(int x, int y, const Color &sRGBCol, double z) const {
    int idx = getIndex(x, y);
    double bufferZ = zBuffer[idx];
    // 离屏幕越近深度越小，写入
    if (z > bufferZ) {
        colorBuffer[idx] = sRGBCol;
        return true;
    }
    std::cout << "z write failed: " << z << std::endl;
    return false;
}

/**
 * 清空缓冲区
 */
void Buffer::clear() const {
    std::fill_n(colorBuffer, bufferWidth * bufferHeight, Color{0, 0, 0, 0});
    std::fill_n(zBuffer, bufferWidth * bufferHeight, 0.0);
}

int Buffer::getIndex(int x, int y) const {
    return x + y * bufferWidth;
}
