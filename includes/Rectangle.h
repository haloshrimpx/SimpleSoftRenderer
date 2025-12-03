//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_RECTANGLE_H
#define SOFTRENDERER_RECTANGLE_H
#include "Vector.h"

namespace geometry {
    /**
* 平面矩形坐标，原点在左下角，y轴向上
*/
    class Rectangle {
    public:
        /*
         * ^ y
         * |
         * | height
         * |
         * Pos-------------> x
         *      width
         */
        double width; // 宽度
        double height; // 高度
        maths::Vector2 originPoint; // 原点坐标

        Rectangle(const maths::Vector2 &origin, double width, double height);

        /**
         * @return 左上角坐标
         */
        maths::Vector2 calcUpperLeftPosition() const;

        /**
         *
         * @return 右上角坐标
         */
        maths::Vector2 calcUpperRightPosition() const;

        /**
         *
         * @return 右下角坐标
         */
        maths::Vector2 calcLowerRightPosition() const;

        void print() const;
    };
}
#endif //SOFTRENDERER_RECTANGLE_H
