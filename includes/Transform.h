//
// Created by Haloshrimp on 2025/12/2.
//

#ifndef SOFTRENDERER_TRANSFORM_H
#define SOFTRENDERER_TRANSFORM_H
#include "Vector.h"

/**
    * 物体的变换
    */
class Transform {
public:
    Transform();

    Transform(const maths::Vector3 &pos, const maths::Vector3 &rot, const maths::Vector3 &scl);

    ~Transform();

    void setPosition(const maths::Vector3 &pos);

    void setRotation(const maths::Vector3 &rot);

    void setScale(const maths::Vector3 &scl);

    void rotate(const maths::Vector3 &delta);

    void move(const maths::Vector3 &delta);

    void scale(const maths::Vector3 &delta);

    void rotate(double xDelta, double yDelta, double zDelta);

    void move(double xDelta, double yDelta, double zDelta);

    void scale(double xDelta, double yDelta, double zDelta);

    maths::Vector3 forwardVec() const;

    maths::Vector3 rightVec();

    maths::Vector3 upVec();

    const maths::Vector3 &getPosition() const;

    const maths::Vector3 &getRotation() const;

    const maths::Vector3 &getScale() const;

    void print() const;

private:
    maths::Vector3 position;
    maths::Vector3 rotation;
    maths::Vector3 scaling;
};

#endif //SOFTRENDERER_TRANSFORM_H
