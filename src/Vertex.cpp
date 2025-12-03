//
// Created by Haloshrimp on 2025/12/2.
//

#include "../includes/Vertex.h"

#include <iostream>
#include  <sstream>

namespace geometry {
    Vertex::Vertex() = default;

    Vertex::Vertex(const Vertex &vert) {
        pos = vert.pos;
        vertNormal = vert.vertNormal;
        vertColor = vert.vertColor;
    }

    Vertex::Vertex(const maths::Vector3 &vec) {
        pos = vec.toVector4(1);
    }

    Vertex::Vertex(const maths::Vector4 &vec) {
        pos = vec;
    }

    Vertex::Vertex(const maths::Vector4 &vec, const maths::Vector3 &normal) {
        pos = vec;
        vertNormal = normal;
    }

    Vertex::Vertex(const maths::Vector3 &vec, const maths::Vector3 &normal, const Color &color) {
        pos = vec.toVector4(1);
        vertNormal = normal;
        vertColor = color;
    }

    Vertex::Vertex(const maths::Vector4 &vec, const maths::Vector3 &normal, const Color &color) {
        pos = vec;
        vertNormal = normal;
        vertColor = color;
    }

    Vertex Vertex::lerp(const Vertex &a, const Vertex &b, const double t) {
        Vertex res;
        res.pos = maths::Vector4::lerp(a.pos, b.pos, t);
        res.vertNormal = maths::Vector3::lerp(a.vertNormal, b.vertNormal, t);
        res.vertColor = Color::lerp(a.vertColor, b.vertColor, t);
        return res;
    }


    double &Vertex::operator[](const int index) {
        return pos[index];
    }

    maths::Vector4 Vertex::operator-(const Vertex &vertex) const {
        return pos - vertex.pos;
    }

    std::string Vertex::formatVertex() const {
        std::stringstream ss;
        //v-Numx.00000Numy.00000Numz.00000
        ss << "v-";
        ss << pos[0];
        ss << pos[1];
        ss << pos[2];
        return ss.str();
    }

    void Vertex::print() const {
        std::clog << "vertex :";
        pos.print();
    }
}
