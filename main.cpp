#include <easyx.h>
#include <iostream>
#include <vector>
#include <conio.h>

#include "includes/Buffer.h"
#include "includes/obj_reader.h"
#include "includes/Camera.h"
#include "includes/Light.h"
#include "includes/Object.h"
#include "includes/shader_render.h"
#include "includes/Transform.h"

std::vector<std::array<geom::Vertex, 3> > generateVertForTest(
    const std::vector<std::array<maths::Vector3, 3> > &verts) {
    std::vector<std::array<geom::Vertex, 3> > output;
    // 遍历顶点组，获取array
    for (const auto &vert: verts) {
        std::array<geom::Vertex, 3> vertArr;
        for (int i = 0; i < 3; ++i) {
            geom::Vertex newVert;
            newVert.pos = vert[i].toVector4(1);

            vertArr[i] = newVert;
        }

        output.push_back(vertArr);
    }

    return output;
}

int main() {
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;

    std::clog.rdbuf(nullptr);

    // 世界空间变换
    Transform cubeTransform(
        {0, 0, 0},
        {20, 0, 20},
        {1, 1, 1}
    );

    Transform cameraWorldTransform(
        {0, 0, 3},
        {0, 0, 0}, // 左手坐标系中，绕y+轴顺时针旋转为正方向
        {1, 1, 1}
    );

    Camera camera(60, 0.5, 100, SCREEN_WIDTH, SCREEN_HEIGHT, cameraWorldTransform);

    geom::Mesh mesh;
    // 读取文件
    if (objreader::tryReadObjFile("Susanna.obj", mesh))
        std::clog << "READ SUCCESS" << std::endl;
    else return 1;

    mesh.print();

    Material mat(COLOR_WHITE, COLOR_BLACK, 32);
    Object cubeTri(mesh, cubeTransform, mat);
    DirectionalLight sun(
        COLOR_WHITE, 1.0, {
            {0, 0, 0}, {0, 0, 0}, {1, 1, 1}
        }
    );

    Buffer renderBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

    // 开始渲染
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);

    while (true) {
        Camera renderCam(60, 0.01, 100, SCREEN_WIDTH, SCREEN_HEIGHT, cameraWorldTransform);
        cubeTransform.rotate(0, 10, 0);
        Object renderCube(cubeTri.mesh, cubeTransform, mat);

        shader::vertexShadingPipeline(renderCube, sun, renderCam, renderBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);

        const maths::Vector3 &camPos = cameraWorldTransform.getPosition();
        const maths::Vector3 &camRot = cameraWorldTransform.getRotation();
        const maths::Vector3 &objRotation = cubeTransform.getRotation();
        std::cout << "\n cam position: " << camPos.x << "\t" << camPos.y << "\t" << camPos.z << std::endl;
        std::cout << "\n cam position: " << camRot.x << "\t" << camRot.y << "\t" << camRot.z << std::endl;
        std::cout << "\n CUBE rotation: " << objRotation.x << "\t" << objRotation.y << "\t" << objRotation.z <<
                std::endl;

        std::cout << "WAITING FOR INPUT:" << std::endl;
        char p = _getch();
        if (p == 'w') {
            // 向前
            cameraWorldTransform.move({0, 0, 0.5});
        } else if (p == 's') {
            cameraWorldTransform.move({0, 0, -0.5});
        } else if (p == 'a') {
            cameraWorldTransform.move({-0.5, 0, 0});
        } else if (p == 'd') {
            cameraWorldTransform.move({0.5, 0, 0});
        } else if (p == 'q') {
            // 向上
            cameraWorldTransform.move({0, 0.5, 0});
        } else if (p == 'e') {
            // 向下
            cameraWorldTransform.move({0, -0.5, 0});
        } else if (p == 'i') {
            // 向下旋转
            cameraWorldTransform.rotate({-5, 0, 0});
        } else if (p == 'k') {
            // 向上旋转
            cameraWorldTransform.rotate({5, 0, 0});
        } else if (p == 'j') {
            // 向左旋转
            cameraWorldTransform.rotate({0, 5, 0});
        } else if (p == 'l') {
            // 向右旋转
            cameraWorldTransform.rotate({0, -5, 0});
        }
    }
}
