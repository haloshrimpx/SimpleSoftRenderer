//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_RECTANGLE_H
#define SOFTRENDERER_RECTANGLE_H
#include "Vector.h"

namespace geom {
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
        int width; // 宽度
        int height; // 高度

        // 原点坐标
        int originX;
        int originY;

        Rectangle(int originX, int originY, int width, int height);

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

        maths::Vector2 getOriginPoint() const;
    };
}
#endif //SOFTRENDERER_RECTANGLE_H
