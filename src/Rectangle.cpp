//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Rectangle.h"

#include <iostream>

namespace geometry {
    Rectangle::Rectangle(const maths::Vector2 &origin, const double width, const double height) {
        originPoint = origin;
        this->height = height;
        this->width = width;
    }

    maths::Vector2 Rectangle::calcUpperLeftPosition() const {
        return {originPoint.x, originPoint.y + width};
    }

    maths::Vector2 Rectangle::calcUpperRightPosition() const {
        return {originPoint.x + width, originPoint.y + width};
    }

    maths::Vector2 Rectangle::calcLowerRightPosition() const {
        return {originPoint.x + width, originPoint.y};
    }

    void Rectangle::print() const {
        std::clog << "Rectangle: width = " << width << "\t height = " << height << std::endl;
        originPoint.print();
    }
}
