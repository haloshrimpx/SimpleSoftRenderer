//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_MATRIX_H
#define SOFTRENDERER_MATRIX_H
#include "Vector.h"

namespace maths {
    class Matrix3x3;
    class Matrix4x4;


    class Matrix3x3 {
    public:
        Matrix3x3();

        explicit Matrix3x3(const Vector3 vectors[3]);

        explicit Matrix3x3(const double arr[3][3]);

        Matrix3x3(const Vector3 &a, const Vector3 &b, const Vector3 &c);

        ~ Matrix3x3() = default;

        double &valueOf(int row, int col);

        const double &getValue(int row, int col) const;

        static Matrix3x3 multiply(const Matrix3x3 &left, const Matrix3x3 &right);

        static Vector3 multiply(const Matrix3x3 &left, const Vector3 &right);

        Matrix3x3 transpose() const;

        Matrix3x3 inverse() const;

        Matrix3x3 operator*(double num) const;

        void print();

    private:
        double matr[3][3]{};

        void swapRow(int r1, int r2);

        void multiplyNumToRow(double num, int row);

        void addRowToAnother(int r1, int r2, double mulNum = 1);
    };

    static const Matrix3x3 IDENTITY_MATRIX_3X3{
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };

    class Matrix4x4 {
    public:
        Matrix4x4();

        explicit Matrix4x4(const double arr[4][4]);

        explicit Matrix4x4(const Vector4 vectors[4]);

        Matrix4x4(const Vector4 &a, const Vector4 &b, const Vector4 &c, const Vector4 &d);

        ~ Matrix4x4() = default;

        double &valueOf(int row, int col);

        const double &getValue(int row, int col) const;

        static Matrix4x4 multiply(const Matrix4x4 &left, const Matrix4x4 &right);

        static Vector4 multiply(const Matrix4x4 &left, const Vector4 &right);

        static Vector3 multiply(const Matrix4x4 &left, const Vector3 &right);

        Matrix4x4 transpose() const;

        Matrix4x4 inverse() const;

        Matrix4x4 operator*(double num) const;

        void print() const;

    private:
        double matr[4][4]{};

        void swapRow(int r1, int r2);

        void multiplyNumToRow(double num, int row);

        void addRowToAnother(int r1, int r2, double mulNum = 1);
    };

    static const Matrix4x4 IDENTITY_MATRIX_4X4 = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
}


#endif //SOFTRENDERER_MATRIX_H
