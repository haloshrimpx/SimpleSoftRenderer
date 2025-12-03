//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Transform.h"

#include <iostream>
#include <cmath>

#include "../includes/Vector.h"

Transform::Transform() {
    position = {0, 0, 0};
    rotation = {0, 0, 0};
    scaling = {1, 1, 1};
}

Transform::Transform(const maths::Vector3 &pos, const maths::Vector3 &rot, const maths::Vector3 &scl) {
    position = pos;
    rotation = rot;
    scaling = scl;
}

Transform::~Transform() = default;

void Transform::setPosition(const maths::Vector3 &pos) {
    position = pos;
}

void Transform::setRotation(const maths::Vector3 &rot) {
    rotation = rot;
}

void Transform::setScale(const maths::Vector3 &scl) {
    scaling = scl;
}

static double clampRotateValue(double val) {
    double normalized = std::fmod(val, 360);

    // 确保结果在 [0, 360) 范围内
    if (normalized < 0)
        normalized += 360;

    return normalized;
}

void Transform::rotate(const maths::Vector3 &delta) {
    rotation = rotation + delta;
    rotation.x = clampRotateValue(rotation.x);
    rotation.y = clampRotateValue(rotation.y);
    rotation.z = clampRotateValue(rotation.z);
}

void Transform::move(const maths::Vector3 &delta) {
    position = position + delta;
}

void Transform::scale(const maths::Vector3 &delta) {
    scaling = scaling + delta;
}

void Transform::rotate(const double xDelta, const double yDelta, const double zDelta) {
    maths::Vector3 delta(xDelta, yDelta, zDelta);
    rotation = rotation + delta;
    rotation.x = clampRotateValue(rotation.x);
    rotation.y = clampRotateValue(rotation.y);
    rotation.z = clampRotateValue(rotation.z);
}

void Transform::move(const double xDelta, const double yDelta, const double zDelta) {
    maths::Vector3 delta(xDelta, yDelta, zDelta);
    position = position + delta;
}

void Transform::scale(const double xDelta, const double yDelta, const double zDelta) {
    maths::Vector3 delta(xDelta, yDelta, zDelta);
    scaling = scaling + delta;
}

maths::Vector3 Transform::forwardVec() const {
    // x = sin(yaw) * cos(pitch)
    // y = sin(pitch)
    // z = cos(yaw) * cos(pitch)

    return {
        std::sin(rotation.y) * std::cos(rotation.x),
        std::sin(rotation.y),
        std::cos(rotation.y) * std::cos(rotation.x)
    };
}

maths::Vector3 Transform::rightVec() {
    //Right = (-sin(yaw),cos(yaw),0)
    double x = -std::sin(rotation.y);
    double y = std::cos(rotation.y);
    return {x, y, 0};
}

maths::Vector3 Transform::upVec() {
    //Up = (-sin(pitch) * cos(yaw),-sin(pitch) * sin(yaw),cos(pitch))
    return maths::VECTOR3_UP;
}

const maths::Vector3 &Transform::getPosition() const {
    return position;
}

const maths::Vector3 &Transform::getRotation() const {
    return rotation;
}

const maths::Vector3 &Transform::getScale() const {
    return scaling;
}

void Transform::print() const {
    std::clog << "Transform: " << std::endl;
    std::clog << ">>position: " << std::endl;
    position.print();
    std::clog << ">>rotation: " << std::endl;
    rotation.print();
    std::clog << "scale: " << std::endl;
    scaling.print();
}
