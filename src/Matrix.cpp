//
// Created by Haloshrimp on 2025/11/23.
//


#include "../includes/Matrix.h"

#include <iostream>
#include <stdexcept>

void swap(double *a, double *b) noexcept {
    double tmp = *a;
    *a = *b;
    *b = tmp;
}

namespace maths {
    /********************** 3x3的方阵 ***************************/

    Matrix3x3::Matrix3x3() {
        for (auto &i: matr) {
            for (double &j: i) {
                j = 0.0;
            }
        }
    }

    Matrix3x3::Matrix3x3(const Vector3 vectors[3]) {
        if (vectors == nullptr)
            throw std::invalid_argument("Input vector array cannot be nullptr");


        for (int i = 0; i < 3; ++i) {
            matr[i][0] = vectors[i].x;
            matr[i][1] = vectors[i].y;
            matr[i][2] = vectors[i].z;
        }
    }

    Matrix3x3::Matrix3x3(const double arr[3][3]) {
        for (int i = 0; i < 3; ++i) {
            if (arr[i] == nullptr) {
                throw std::invalid_argument("Invalid array to construct a matrix");
            }
        }

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                matr[i][j] = arr[i][j];
                // matr[i][1] = arr[i][1];
                // matr[i][2] = arr[i][2];
            }
        }
    }

    Matrix3x3::Matrix3x3(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
        /*
         * matr:
         * a.x, a.y, a.z
         * b.x, b.y, b.z
         * c.x, c.y, c.z
         */

        matr[0][0] = a.x;
        matr[0][1] = a.y;
        matr[0][2] = a.z;

        matr[1][0] = b.x;
        matr[1][1] = b.y;
        matr[1][2] = b.z;

        matr[2][0] = c.x;
        matr[2][1] = c.y;
        matr[2][2] = c.z;
    }

    // get和set值
    double &Matrix3x3::valueOf(const int row, const int col) {
        if (row < 0 || row > 2 || col < 0 || col > 2)
            throw std::out_of_range("access matrix 3x3 out of range");

        return matr[row][col];
    }

    // 只允许set值
    const double &Matrix3x3::getValue(const int row, const int col) const {
        if (row < 0 || row > 2 || col < 0 || col > 2)
            throw std::out_of_range("access matrix 3x3 out of range");

        return matr[row][col];
    }

    // 将a乘b
    Matrix3x3 Matrix3x3::multiply(const Matrix3x3 &left, const Matrix3x3 &right) {
        Matrix3x3 newMatrix;

        // 乘积矩阵第i行第j列处的元素等于 左矩阵的第i行 与 右矩阵的第j列 对应元素乘积 之和
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                double sum = 0;
                for (int k = 0; k < 3; ++k) {
                    sum += left.matr[i][k] * right.matr[k][j];
                }

                if (std::abs(sum) < 1e-10)
                    sum = 0;

                newMatrix.matr[i][j] = sum;
            }
        }

        return newMatrix;
    }

    Vector3 Matrix3x3::multiply(const Matrix3x3 &left, const Vector3 &right) {
        // TODO
        return VECTOR3_ZERO;
    }

    // 转置矩阵，将行和列交换
    Matrix3x3 Matrix3x3::transpose() const {
        Matrix3x3 newMatr;
        double arr[3][3];

        // 将行和列交换
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                arr[i][j] = getValue(j, i);
            }
        }

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                newMatr.valueOf(i, j) = arr[i][j];
            }
        }

        return newMatr;
    }

    Matrix3x3 Matrix3x3::inverse() const {
        Matrix3x3 identityMatrix = IDENTITY_MATRIX_3X3;
        Matrix3x3 newMatrix(matr);

        for (int pivotCol = 0; pivotCol < 3; ++pivotCol) {
            // 寻找主元行
            int pivotRow = pivotCol;
            for (int i = pivotCol + 1; i < 3; ++i) {
                if (std::abs(newMatrix.matr[i][pivotCol]) > std::abs(newMatrix.matr[pivotRow][pivotCol])) {
                    pivotRow = i;
                }
            }

            double pivotValue = newMatrix.matr[pivotRow][pivotCol];

            // 检查是否可逆
            if (std::abs(pivotValue) < 1e-10) {
                throw std::bad_exception();
            }

            // 交换行（如果需要）
            if (pivotRow != pivotCol) {
                newMatrix.swapRow(pivotCol, pivotRow);
                identityMatrix.swapRow(pivotCol, pivotRow);
            }

            // 归一化主元行
            newMatrix.multiplyNumToRow(1.0 / pivotValue, pivotCol);
            identityMatrix.multiplyNumToRow(1.0 / pivotValue, pivotCol);

            // 消元：对其他行进行消元
            for (int row = 0; row < 3; ++row) {
                if (row != pivotCol) {
                    double factor = -newMatrix.matr[row][pivotCol];
                    newMatrix.addRowToAnother(pivotCol, row, factor);
                    identityMatrix.addRowToAnother(pivotCol, row, factor);
                }
            }
        }

        return identityMatrix;
    }

    Matrix3x3 Matrix3x3::operator*(const double num) const {
        Matrix3x3 newMatrix(matr);

        for (auto &i: newMatrix.matr) {
            for (double &val: i) {
                val *= num;
            }
        }
        return newMatrix;
    }

    void Matrix3x3::print() {
        std::clog << "Matrix 3x3:" << std::endl;

        for (auto &i: matr) {
            std::cout << i[0] << "\t" << i[1] << "\t" << i[2] << "\t" << std::endl;
        }
    }

    void Matrix3x3::swapRow(int r1, int r2) {
        if (r1 < 0 || r1 > 2 || r2 < 0 || r2 > 2)
            return;

        for (int i = 0; i < 3; ++i) {
            swap(&matr[r1][i], &matr[r2][i]);
        }
    }

    void Matrix3x3::multiplyNumToRow(double num, int row) {
        if (num == 0)
            return;

        if (row < 0 || row > 2)
            return;

        for (int i = 0; i < 3; ++i) {
            double &val = matr[row][i];
            val *= num;

            if (std::abs(val) < 1e-10)
                val = 0;
        }
    }

    /**
     * 将r1的值乘上mulNum，再加到r2上去
     *
     * @param r1
     * @param r2
     * @param mulNum
     */
    void Matrix3x3::addRowToAnother(int r1, int r2, double mulNum) {
        if (mulNum == 0)
            return;

        for (int i = 0; i < 3; ++i) {
            matr[r2][i] += matr[r1][i] * mulNum;

            if (std::abs(matr[r2][i]) < 1e-10)
                matr[r2][i] = 0;
        }
    }

    /********************** 4x4的方阵 ***************************/

    Matrix4x4::Matrix4x4() {
        for (auto &i: matr) {
            for (double &j: i) {
                j = 0.0;
            }
        }
    }

    Matrix4x4::Matrix4x4(const double arr[4][4]) {
        for (int i = 0; i < 4; ++i) {
            if (arr[i] == nullptr) {
                throw std::invalid_argument("Invalid array to construct a matrix");
            }
        }

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                matr[i][j] = arr[i][j];
            }
        }
    }

    Matrix4x4::Matrix4x4(const Vector4 vectors[4]) {
        if (vectors == nullptr)
            throw std::invalid_argument("Input vector array cannot be nullptr");


        for (int i = 0; i < 3; ++i) {
            matr[i][0] = vectors[i].x;
            matr[i][1] = vectors[i].y;
            matr[i][2] = vectors[i].z;
            matr[i][3] = vectors[i].w;
        }
    }

    Matrix4x4::Matrix4x4(const Vector4 &a, const Vector4 &b, const Vector4 &c, const Vector4 &d) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                Vector4 currentVec;
                switch (i) {
                    case 0: currentVec = a;
                        break;
                    case 1: currentVec = b;
                        break;
                    case 2: currentVec = c;
                        break;
                    case 3: currentVec = d;
                        break;
                    default: throw std::out_of_range("accessing current vector out of range");
                }

                matr[i][j] = currentVec[j];
            }
        }
    }

    Matrix4x4 getRotaionMatrixX(double degree);

    double &Matrix4x4::valueOf(int row, int col) {
        if (row < 0 || row > 3 || col < 0 || col > 3)
            throw std::out_of_range("access matrix 3x3 out of range");

        return matr[row][col];
    }

    const double &Matrix4x4::getValue(int row, int col) const {
        if (row < 0 || row > 3 || col < 0 || col > 3)
            throw std::out_of_range("access matrix 3x3 out of range");

        return matr[row][col];
    }

    Matrix4x4 Matrix4x4::multiply(const Matrix4x4 &left, const Matrix4x4 &right) {
        Matrix4x4 newMatrix;

        // C[i][j] = ∑(A[i][k] × B[k][j])
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                double sum = 0;
                for (int k = 0; k < 4; ++k) {
                    sum += left.matr[i][k] * right.matr[k][j];
                }

                if (std::abs(sum) < 1e-10)
                    sum = 0;

                newMatrix.matr[i][j] = sum;
            }
        }

        return newMatrix;
    }

    /**
     * 矩阵右乘列向量
     *
     * @param left
     * @param right
     * @return
     */
    Vector4 Matrix4x4::multiply(const Matrix4x4 &left, const Vector4 &right) {
        Vector4 newVector;
        std::array<double, 4> vecArr = right.toArray();

        // C[i][0] = ∑(A[i][k] × B[k][0])
        for (int i = 0; i < 4; ++i) {
            double sum = 0;
            for (int j = 0; j < 4; ++j) {
                sum += left.matr[i][j] * vecArr[j];
            }

            if (std::abs(sum) < 1e-10)
                sum = 0;

            newVector[i] = sum;
        }

        return newVector;
    }

    /**
    * 矩阵右乘列向量
    *
    * @param left 变换矩阵
    * @param right 要变换的向量，默认w分量为1 (点的形式)
    * @return
    */
    Vector3 Matrix4x4::multiply(const Matrix4x4 &left, const Vector3 &right) {
        Vector3 newVector;
        std::array vecArr = right.toVector4(1).toArray();

        // C[i][0] = ∑(A[i][k] × B[k][0])
        for (int i = 0; i < 3; ++i) {
            double sum = 0;
            for (int j = 0; j < 4; ++j) {
                sum += left.matr[i][j] * vecArr[j];
            }

            if (std::abs(sum) < 1e-10)
                sum = 0;

            newVector[i] = sum;
        }

        return newVector;
    }


    Matrix4x4 Matrix4x4::transpose() const {
        Matrix4x4 newMatr;
        double arr[4][4];

        // 将行和列交换
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                arr[i][j] = getValue(j, i);
            }
        }

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                newMatr.valueOf(i, j) = arr[i][j];
            }
        }

        return newMatr;
    }

    /**
     * 高斯-若尔当消元法求逆矩阵
     * @return
     */
    Matrix4x4 Matrix4x4::inverse() const {
        Matrix4x4 identityMatrix = IDENTITY_MATRIX_4X4;
        Matrix4x4 newMatrix(matr);

        for (int pivotCol = 0; pivotCol < 4; ++pivotCol) {
            // 寻找主元行
            int pivotRow = pivotCol;
            for (int i = pivotCol + 1; i < 4; ++i) {
                if (std::abs(newMatrix.matr[i][pivotCol]) > std::abs(newMatrix.matr[pivotRow][pivotCol])) {
                    pivotRow = i;
                }
            }

            double pivotValue = newMatrix.matr[pivotRow][pivotCol];

            // 检查是否可逆
            if (std::abs(pivotValue) < 1e-10) {
                throw std::bad_exception();
            }

            // 交换行
            if (pivotRow != pivotCol) {
                newMatrix.swapRow(pivotCol, pivotRow);
                identityMatrix.swapRow(pivotCol, pivotRow);
            }

            // 归一化主元行
            newMatrix.multiplyNumToRow(1.0 / pivotValue, pivotCol);
            identityMatrix.multiplyNumToRow(1.0 / pivotValue, pivotCol);

            // 消元：对其他行进行消元
            for (int row = 0; row < 4; ++row) {
                if (row != pivotCol) {
                    double factor = -newMatrix.matr[row][pivotCol];
                    newMatrix.addRowToAnother(pivotCol, row, factor);
                    identityMatrix.addRowToAnother(pivotCol, row, factor);
                }
            }
        }

        return identityMatrix;
    }

    Matrix4x4 Matrix4x4::operator*(double num) const {
        Matrix4x4 newMatrix(matr);

        for (auto &i: newMatrix.matr) {
            for (double &val: i) {
                val *= num;
            }
        }
        return newMatrix;
    }

    void Matrix4x4::print() const {
        std::cout << "Matrix 4X4:" << std::endl;

        for (auto &i: matr) {
            std::cout << i[0] << "\t" << i[1] << "\t" << i[2] << "\t" << i[3] << "\t" << std::endl;
        }
    }

    void Matrix4x4::swapRow(int r1, int r2) {
        if (r1 < 0 || r1 > 3 || r2 < 0 || r2 > 3)
            return;

        for (int i = 0; i < 4; ++i) {
            swap(&matr[r1][i], &matr[r2][i]);
        }
    }

    void Matrix4x4::multiplyNumToRow(double num, int row) {
        if (num == 0)
            return;

        if (row < 0 || row > 3)
            return;

        for (int i = 0; i < 4; ++i) {
            double &val = matr[row][i];
            val *= num;

            if (std::abs(val) < 1e-10)
                val = 0;
        }
    }

    void Matrix4x4::addRowToAnother(int r1, int r2, double mulNum) {
        if (mulNum == 0)
            return;

        for (int i = 0; i < 4; ++i) {
            matr[r2][i] += matr[r1][i] * mulNum;

            if (std::abs(matr[r2][i]) < 1e-10)
                matr[r2][i] = 0;
        }
    }
}
