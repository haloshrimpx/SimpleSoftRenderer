//
// Created by Haloshrimp on 2025/12/4.
//

#ifndef SOFTRENDERER_BUFFER_H
#define SOFTRENDERER_BUFFER_H
#include "Color.h"

class Buffer {
public:
    Buffer(int width, int height);

    virtual ~Buffer() = default;

    [[nodiscard]] int getBufferSize() const;

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    [[nodiscard]] int getIndex(int x, int y) const;

    virtual void clear() const = 0;

private:
    int bufferWidth, bufferHeight;
};

class ScreenBuffer final : public Buffer {
public:
    ScreenBuffer(int width, int height);

    ~ScreenBuffer() override;

    [[nodiscard]] const double *getZBuffer() const;

    [[nodiscard]] const Color *getColorBuffer() const;

    [[nodiscard]] bool writeColorBuffer(int x, int y, const Color &sRGBCol, double z) const;

    void clear() const override;

private:
    Color *colorBuffer;
    double *zBuffer;
};

class DepthBuffer final : public Buffer {
public:
    DepthBuffer(int width, int height);

    ~DepthBuffer() override;

    bool writeDepthBuffer(int x, int y, double z) const;

    [[nodiscard]] double getDepth(int x, int y) const;

    void clear() const override;

private:
    double *depthBuffer;
};

#endif //SOFTRENDERER_BUFFER_H
