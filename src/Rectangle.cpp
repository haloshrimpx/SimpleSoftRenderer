//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Rectangle.h"

#include <iostream>

namespace geom {
    Rectangle::Rectangle(int originX, int originY, int width, int height) {
        this->originX = originX;
        this->originY = originY;
        this->width = width;
        this->height = height;
    }

    maths::Vector2 Rectangle::calcUpperLeftPosition() const {
        return {static_cast<double>(originX), static_cast<double>(originY + width)};
    }

    maths::Vector2 Rectangle::calcUpperRightPosition() const {
        return {static_cast<double>(originX + width), static_cast<double>(originY + width)};
    }

    maths::Vector2 Rectangle::calcLowerRightPosition() const {
        return {static_cast<double>(originX + width), static_cast<double>(originY)};
    }

    maths::Vector2 Rectangle::getOriginPoint() const {
        return {static_cast<double>(originX), static_cast<double>(originY)};
    }
}
