//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_VERTEX_H
#define SOFTRENDERER_VERTEX_H
#include "Vector.h"
#include <string>

#include "Color.h"

namespace maths {
    class Vector3;
    class Vector4;
}

namespace geometry {
    /**
   * 顶点结构体
   */
    class Vertex {
    public:
        // 顶点位置
        maths::Vector4 pos;
        maths::Vector3 vertNormal;
        Color vertColor;

        Vertex();

        Vertex(const Vertex &vert);

        explicit Vertex(const maths::Vector3 &vec);

        explicit Vertex(const maths::Vector4 &vec);

        Vertex(const maths::Vector4 &vec, const maths::Vector3 &normal);

        Vertex(const maths::Vector3 &vec, const maths::Vector3 &normal, const Color &color);

        Vertex(const maths::Vector4 &vec, const maths::Vector3 &normal, const Color &color);

        static Vertex lerp(const Vertex &a, const Vertex &b, double t);

        double &operator[](int index);

        maths::Vector4 operator-(const Vertex &vertex) const;

        std::string formatVertex() const;

        void print() const;
    };
}

#endif //SOFTRENDERER_VERTEX_H
