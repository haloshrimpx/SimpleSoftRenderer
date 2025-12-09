//
// Created by Haloshrimp on 2025/12/4.
//

#include "../includes/obj_reader.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace objreader {
    bool tryReadObjFile(const std::string &path, geom::Mesh &output) {
        // 尝试打开文件
        std::ifstream objFile(path);
        if (!objFile.is_open()) {
            output = geom::Mesh();
            return false;
        }

        // 逐行读取文件
        std::vector<geom::Triangle> triangles;
        std::vector<geom::Vertex> vertices;
        std::vector<maths::Vector3> normals;

        std::string line;
        while (std::getline(objFile, line)) {
            // 读取顶点
            // 注意字符空格
            if (line.starts_with("v ")) {
                maths::Vector3 vertPos = parseVertexPosition(line);
                vertices.emplace_back(vertPos);
            }
            // 读取顶点法线
            else if (line.starts_with("vn ")) {
                maths::Vector3 vertNormal = parseVertexNormal(line);
                normals.emplace_back(vertNormal);
            }
            // 读取面数据
            else if (line.starts_with("f ")) {
                geom::Triangle triangle = parseFace(line);
                triangles.emplace_back(triangle);
            }
            // 跳过其它行
            else {
                std::cout << "Skip or invalid line: -->" << line << "<--" << std::endl;
            }
        }

        output = geom::Mesh(vertices, triangles, normals);

        return true;
    }

    maths::Vector3 parseVertexPosition(const std::string &line) {
        // 格式:
        // v x y z

        std::istringstream lineStream(line);
        std::string token;
        lineStream >> token; // 跳过 "v"

        double x, y, z;
        lineStream >> x >> y >> z;
        return {x, y, z};
    }

    maths::Vector3 parseVertexNormal(const std::string &line) {
        // 格式:
        // vn x y z

        std::istringstream lineStream(line);
        std::string token;
        lineStream >> token; // 跳过 "vn"

        double x, y, z;
        lineStream >> x >> y >> z;
        return {x, y, z};
    }

    geom::Triangle parseFace(const std::string &line) {
        // 格式
        // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3

        geom::Triangle triangle;
        std::istringstream iss(line);
        std::string token;

        // 跳过 "f"
        iss >> token;

        int counter = 0;

        // 读取每个顶点定义
        while (iss >> token) {
            int v = 0;
            int vt = 0;
            int vn = 0;

            std::istringstream vertexStream(token);
            std::string part;

            // 解析 v
            if (std::getline(vertexStream, part, '/')) {
                v = std::stoi(part);

                // 解析 vt
                if (std::getline(vertexStream, part, '/')) {
                    // 跳过 vt
                    if (!part.empty()) {
                        vt = std::stoi(part);
                    }

                    // 解析 vn
                    if (std::getline(vertexStream, part, '/')) {
                        if (!part.empty()) {
                            vn = std::stoi(part);
                        }
                    }
                }
            }

            // 存储顶点索引
            triangle.vertIndices[counter] = v - 1;
            triangle.normalIndices[counter] = vn - 1;
            counter++;
        }

        return triangle;
    }
}
