//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Vector.h"

#include <array>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

double square(const double value) {
    return value * value;
}

namespace maths {
    /*
     * 二维向量
     */

    Vector2::Vector2(const double x, const double y) {
        this->x = x;
        this->y = y;
    }

    Vector2::Vector2() {
        x = 0;
        y = 0;
    }

    Vector2::Vector2(const Vector2 &other) {
        x = other.x;
        y = other.y;
    }

    Vector2::~Vector2() = default;

    Vector2 Vector2::operator+(const Vector2 &v) const {
        Vector2 res;
        res.x = x + v.x;
        res.y = y + v.y;
        return res;
    }

    Vector2 Vector2::operator-(const Vector2 &v) const {
        Vector2 res;
        res.x = x - v.x;
        res.y = y - v.y;
        return res;
    }

    Vector2 Vector2::operator*(const double n) const {
        Vector2 res;
        res.x = x * n;
        res.y = y * n;
        return res;
    }

    double &Vector2::operator[](int index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("accessing value in Vector2 index out of range");
        }
    }

    double Vector2::operator[](const int index) const {
        double value;

        switch (index) {
            case 0: value = x;
                break;
            case 1: value = y;
                break;
            default: throw std::out_of_range("accessing value in Vector3 index out of range");
        }

        return value;
    }

    Vector2 operator*(const double n, const Vector2 &v) {
        Vector2 res;
        res.x = v.x * n;
        res.y = v.y * n;
        return res;
    }

    double Vector2::dot(const Vector2 &a, const Vector2 &b) {
        // x1 * x2 + y1 * y2
        return a.x * b.x
               + a.y * b.y;
    }

    double Vector2::cross(const Vector2 &a, const Vector2 &b) {
        //  u = (u₁, u₂) 和 v = (v₁, v₂)
        // u × v = u₁v₂ - u₂v₁

        return a.x * b.y - a.y * b.x;
    }

    double Vector2::distance(const Vector2 &a, const Vector2 &b) {
        return sqrt(square(a.x - b.x)
                    + square(a.y - b.y));
    }

    double Vector2::getAngle(const Vector2 &a, const Vector2 &b) {
        double degCos = dot(a, b) / (a.getMagnitude() * b.getMagnitude()); // 向量夹角的余弦值
        double radArccos = acos(degCos); // 求反
        return radArccos * 180.0 / std::numbers::pi; // 转换为角度制
    }

    void Vector2::print() const {
        std::clog << " x = " << x << ", y = " << y << std::endl;
    }

    double Vector2::getMagnitude() const {
        // 根号下 x^2 + y^2
        return std::sqrt(x * x + y * y);
    }

    Vector2 Vector2::normalize() const {
        Vector2 res;
        double mag = getMagnitude();
        res.x = x / mag;
        res.y = y / mag;
        return res;
    }

    Vector3 Vector2::toVector3(const double z) const {
        return Vector3{x, y, z};
    }

    std::array<double, 2> Vector2::toArray() const {
        return std::array{x, y};
    }

    /*
     * 三维向量
     */

    Vector3::Vector3(const double x, const double y, const double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector3::Vector3() {
        x = 0;
        y = 0;
        z = 0;
    }

    Vector3::Vector3(const Vector3 &other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    Vector3::~Vector3() = default;

    Vector3 Vector3::operator+(const Vector3 &v) const {
        Vector3 res;
        res.x = x + v.x;
        res.y = y + v.y;
        res.z = z + v.z;
        return res;
    }

    Vector3 Vector3::operator-(const Vector3 &v) const {
        Vector3 res;
        res.x = x - v.x;
        res.y = y - v.y;
        res.z = z - v.z;
        return res;
    }

    Vector3 Vector3::operator*(double n) const {
        Vector3 res;
        res.x = x * n;
        res.y = y * n;
        res.z = z * n;
        return res;
    }

    double &Vector3::operator[](int index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("accessing value in Vector3 index out of range");
        }
    }

    double Vector3::operator[](const int index) const {
        double value;

        switch (index) {
            case 0: value = x;
                break;
            case 1: value = y;
                break;
            case 2: value = z;
                break;
            default: throw std::out_of_range("accessing value in Vector3 index out of range");
        }

        return value;
    }

    Vector3 operator*(const double n, const Vector3 &v) {
        Vector3 res;
        res.x = v.x * n;
        res.y = v.y * n;
        res.z = v.z * n;
        return res;
    }

    double Vector3::dot(const Vector3 &a, const Vector3 &b) {
        return a.x * b.x
               + a.y * b.y
               + a.z * b.z;
    }

    Vector3 Vector3::cross(const Vector3 &a, const Vector3 &b) {
        //对于两个三维向量：
        // a = (a₁, a₂, a₃)
        // b = (b₁, b₂, b₃)
        // 它们的叉乘结果是一个新的三维向量：
        // a × b = (a₂b₃ - a₃b₂, a₃b₁ - a₁b₃, a₁b₂ - a₂b₁)

        Vector3 res;
        res.x = a.y * b.z - b.y * a.z;
        res.y = a.z * b.x - b.z * a.x;
        res.z = a.x * b.y - b.x * a.y;
        return res;
    }

    Vector3 Vector3::lerp(const Vector3 &a, const Vector3 &b, const double t) {
        // V_lerp = (1 - t) * a + t * b
        return (1 - t) * a + t * b;
    }

    const double &Vector3::getValue(const int index) {
        return this->operator[](index);
    }

    Vector4 Vector3::toVector4(const double w) const {
        return Vector4{x, y, z, w};
    }

    Vector2 Vector3::toVector2() const {
        return Vector2{x, y};
    }

    double Vector3::distance(const Vector3 &a, const Vector3 &b) {
        return sqrt(square(a.x - b.x)
                    + square(a.y - b.y)
                    + square(a.z - b.z));
    }

    double Vector3::getAngle(const Vector3 &a, const Vector3 &b) {
        double degCos = dot(a, b) / (a.getMagnitude() * b.getMagnitude()); // 向量夹角的余弦值
        double radArccos = acos(degCos); // 求反
        return radArccos * 180.0 / std::numbers::pi; // 转换为角度制
    }

    std::array<double, 3> Vector3::toArray() const {
        return std::array{x, y, z};
    }

    void Vector3::print() const {
        std::clog << " x = " << x << ",\t y = " << y << ",\t z = " << z << std::endl;
    }

    std::string Vector3::formatVector3() const {
        std::stringstream ss;
        //vec3-NumxNumyNumz
        ss << "vec3-";
        ss << x;
        ss << y;
        ss << z;
        return ss.str();
    }

    double Vector3::getMagnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 Vector3::normalize() const {
        Vector3 res;
        double mag = getMagnitude();
        res.x = x / mag;
        res.y = y / mag;
        res.z = z / mag;
        return res;
    }


    Vector4::Vector4() {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }

    Vector4::Vector4(const Vector3 &vec3, double w) {
        this->x = vec3.x;
        this->y = vec3.y;
        this->z = vec3.z;
        this->w = w;
    }

    Vector4::Vector4(double x, double y, double z, double w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    Vector4::Vector4(const double arr[4]) {
        for (int i = 0; i < 4; ++i) {
            this->operator[](i) = arr[i];
        }
    }

    Vector4::Vector4(const Vector4 &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
    }

    Vector4::~Vector4() = default;

    Vector4 Vector4::operator+(const Vector4 &v) const {
        Vector4 res;
        res.x = x + v.x;
        res.y = y + v.y;
        res.z = z + v.z;
        res.w = w + v.w;
        return res;
    }

    Vector4 Vector4::operator-(const Vector4 &v) const {
        Vector4 res;
        res.x = x - v.x;
        res.y = y - v.y;
        res.z = z - v.z;
        res.w = w - v.w;
        return res;
    }

    Vector4 Vector4::operator*(double n) const {
        Vector4 res;
        res.x = x * n;
        res.y = y * n;
        res.z = z * n;
        res.w = w * n;
        return res;
    }

    double &Vector4::operator[](int index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: throw std::out_of_range("accessing value in Vector3 index out of range");
        }
    }

    double Vector4::operator[](int index) const {
        double value;

        switch (index) {
            case 0: value = x;
                break;
            case 1: value = y;
                break;
            case 2: value = z;
                break;
            case 3: value = w;
                break;
            default: throw std::out_of_range("accessing value in Vector3 index out of range");
        }

        return value;
    }

    Vector4 &Vector4::operator*=(double val) {
        this->x *= val;
        this->y *= val;
        this->z *= val;
        this->w *= val;
        return *this;
    }

    double Vector4::dot(const Vector4 &a, const Vector4 &b) {
        return a.x * b.x
               + a.y * b.y
               + a.z * b.z
               + a.w * b.w;
    }

    Vector4 Vector4::cross(const Vector4 &a, const Vector4 &b) {
        Vector4 res;
        res.x = a.y * b.z - b.y * a.z;
        res.y = a.z * b.x - b.z * a.x;
        res.z = a.x * b.y - b.x * a.y;
        res.w = 0; // 点的叉乘无意义，向量的w为0
        return res;
    }

    Vector4 Vector4::lerp(const Vector4 &a, const Vector4 &b, const double t) {
        return (1 - t) * a + t * b;
    }

    Vector3 Vector4::toVector3() const {
        return {x, y, z};
    }

    Vector2 Vector4::toVector2() const {
        return {x, y};
    }

    std::array<double, 4> Vector4::toArray() const {
        return std::array{x, y, z, w};
    }

    void Vector4::print() const {
        std::clog << "Vector4:" << "\t x = " << x << ",\t y = " << y << ",\t z = " << z << ",\t w = " << w << std::endl;
    }

    Vector4 operator*(const double n, const Vector4 &v) {
        Vector4 res;
        res.x = v.x * n;
        res.y = v.y * n;
        res.z = v.z * n;
        res.w = v.w * n;
        return res;
    }
}
