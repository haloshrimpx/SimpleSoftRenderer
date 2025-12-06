//
// Created by Haloshrimp on 2025/12/4.
//

#ifndef SOFTRENDERER_OBJ_READER_H
#define SOFTRENDERER_OBJ_READER_H
#include "Mesh.h"

namespace objreader {
    bool tryReadObjFile(const std::string &path, geom::Mesh &output);

    maths::Vector3 parseVertexPosition(const std::string &line);

    maths::Vector3 parseVertexNormal(const std::string &line);

    geom::Triangle parseFace(const std::string &line);
}

#endif //SOFTRENDERER_OBJ_READER_H
