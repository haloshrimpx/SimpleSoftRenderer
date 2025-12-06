//
// Created by Haloshrimp on 2025/12/4.
//

#ifndef SOFTRENDERER_BUFFER_H
#define SOFTRENDERER_BUFFER_H
#include "Color.h"

class Buffer {
public:
    Buffer(int width, int height);

    ~Buffer();

    maths::Vector2 getBufferSize() const;

    const double *getZBuffer() const;

    const Color *getColorBuffer() const;

    bool writeColorBuffer(int x, int y, const Color &sRGBCol, double z) const;

    int getIndex(int x, int y) const;

    void clear() const;

private:
    int bufferWidth, bufferHeight;
    Color *colorBuffer;
    double *zBuffer;
};

#endif //SOFTRENDERER_BUFFER_H
