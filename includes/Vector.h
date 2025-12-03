//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_VECTOR_H
#define SOFTRENDERER_VECTOR_H
#include <array>

namespace maths {
    class Vector2;
    class Vector3;
    class Vector4;

    // 二维向量
    class Vector2 {
    public:
        double x;
        double y;

        Vector2(double x, double y);

        Vector2();

        Vector2(const Vector2 &other);

        ~Vector2();

        Vector2 operator+(const Vector2 &v) const;

        Vector2 operator-(const Vector2 &v) const;

        Vector2 operator*(double n) const;

        double &operator[](int index);

        double operator[](int index) const;

        static double dot(const Vector2 &a, const Vector2 &b);

        static double cross(const Vector2 &a, const Vector2 &b);

        static double distance(const Vector2 &a, const Vector2 &b);

        static double getAngle(const Vector2 &a, const Vector2 &b);

        void print() const;

        double getMagnitude() const;

        Vector2 normalize() const;

        Vector3 toVector3(double z = 0) const;

        std::array<double, 2> toArray() const;
    };

    Vector2 operator*(double n, const Vector2 &v);

    /**
     * 三维向量
     */
    class Vector3 {
    public:
        double x;
        double y;
        double z;

        Vector3(double x, double y, double z);

        Vector3();

        Vector3(const Vector3 &other);

        ~Vector3();

        Vector3 operator+(const Vector3 &v) const;

        Vector3 operator-(const Vector3 &v) const;

        Vector3 operator*(double n) const;

        double &operator[](int index);

        double operator[](int index) const;

        static double dot(const Vector3 &a, const Vector3 &b);

        static Vector3 cross(const Vector3 &a, const Vector3 &b);

        static Vector3 lerp(const Vector3 &a, const Vector3 &b, double t);

        const double &getValue(int index);

        Vector4 toVector4(double w) const;

        Vector2 toVector2() const;

        static double distance(const Vector3 &a, const Vector3 &b);

        static double getAngle(const Vector3 &a, const Vector3 &b);

        std::array<double, 3> toArray() const;

        void print() const;

        double getMagnitude() const;

        Vector3 normalize() const;
    };

    Vector3 operator*(double n, const Vector3 &v);

    /**
     * (0,0,0)的简便写法
     */
    const Vector3 VECTOR3_ZERO(0, 0, 0);

    /**
   * (0,1,0)的简便写法
   */
    const Vector3 VECTOR3_UP(0, 1, 0);

    /**
   * (0,0,1)的简便写法
   */
    const Vector3 VECTOR3_FORWARD(0, 0, 1);

    /**
   * (1,0,0)的简便写法
   */
    const Vector3 VECTOR3_RIGHT(1, 0, 0);

    /**
     * 四维向量
     */
    class Vector4 {
    public:
        double x;
        double y;
        double z;
        double w;

        Vector4();

        Vector4(const Vector3 &vec3, double w);

        Vector4(double x, double y, double z, double w);

        explicit Vector4(const double arr[4]);

        Vector4(const Vector4 &other);

        ~Vector4();

        Vector4 operator+(const Vector4 &v) const;

        Vector4 operator-(const Vector4 &v) const;

        Vector4 operator*(double n) const;

        double &operator[](int index);

        double operator[](int index) const;

        static double dot(const Vector4 &a, const Vector4 &b);

        static Vector4 cross(const Vector4 &a, const Vector4 &b);

        static Vector4 lerp(const Vector4 &a, const Vector4 &b, double t);

        Vector3 toVector3() const;

        Vector2 toVector2() const;

        std::array<double, 4> toArray() const;

        void print() const;
    };

    Vector4 operator*(double n, const Vector4 &v);

    const Vector4 VECTOR4_ZERO(0, 0, 0, 0);
    const Vector4 VECTOR4_UP(0, 1, 0, 0);
    const Vector4 VECTOR4_FORWARD(0, 0, 1, 0);
    const Vector4 VECTOR4_RIGHT(1, 0, 0, 0);
}


#endif //SOFTRENDERER_VECTOR_H
