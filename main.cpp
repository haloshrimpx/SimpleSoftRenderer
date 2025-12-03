#include <easyx.h>
#include <iostream>
#include <vector>
#include <conio.h>

#include "includes/Camera.h"
#include "includes/Light.h"
#include "includes/Object.h"
#include "includes/shader_render.h"
#include "includes/shader_transform.h"
#include "includes/Transform.h"

std::vector<std::array<geometry::Vertex, 3> > generateVertForTest(
    const std::vector<std::array<maths::Vector3, 3> > &verts) {
    std::vector<std::array<geometry::Vertex, 3> > output;
    // 遍历顶点组，获取array
    for (const auto &vert: verts) {
        std::array<geometry::Vertex, 3> vertArr;
        for (int i = 0; i < 3; ++i) {
            geometry::Vertex newVert;
            newVert.pos = vert[i].toVector4(1);
            newVert.vertNormal = (vert[i] - maths::VECTOR3_ZERO).normalize();
            newVert.vertColor = Color(1, 0.5, 0.5);

            vertArr[i] = newVert;
        }

        output.push_back(vertArr);
    }

    return output;
}

int main() {
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;

    // std::clog.rdbuf(nullptr);

    // 世界空间变换
    Transform cubeTransform(
        {0, 0, 0},
        {30, 45, 30},
        {1, 1, 1}
    );

    Transform cameraWorldTransform(
        {0, 0, 5},
        {0, 0, 0}, // 左手坐标系中，绕y+轴顺时针旋转为正方向
        {1, 1, 1}
    );

    Camera camera(60, 0.5, 100, SCREEN_WIDTH, SCREEN_HEIGHT, cameraWorldTransform);

    std::vector<std::array<maths::Vector3, 3> > mesh = {
        // Front
        {{{-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}},
        {{{-1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}}},
        // Back
        {{{1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}}},
        {{{1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}}},
        // Right
        {{{1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}}},
        {{{1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}},
        // Left
        {{{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}}},
        {{{-1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f}}},
        // Top
        {{{-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}},
        {{{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f}}},
        // Bottom
        {{{-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, 1.0f}}},
        {{{-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f}}}
    };
    Material mat(COLOR_WHITE, COLOR_BLACK, 32);
    Object cubeTri(generateVertForTest(mesh), cubeTransform, mat);
    DirectionalLight sun(
        COLOR_WHITE, 1.0, {
            {0, 0, 0}, {0, 0, 0}, {1, 1, 1}
        }
    );

    // 开始渲染
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);

    while (true) {
        Camera renderCam(60, 0.01, 100, SCREEN_WIDTH, SCREEN_HEIGHT, cameraWorldTransform);
        cubeTransform.rotate(0, 10, 0);
        Object renderCube(cubeTri.mesh, cubeTransform, mat);

        std::clog << "render start:" << std::endl;
        renderCube.mesh.print();

        cleardevice();

        shader::transformObjToWorldSpace(renderCube);
        std::clog << ">>>>>>>>>>>>>>>>>to world space:" << std::endl;
        renderCube.mesh.print();

        shader::shadingVertex(renderCube, sun, renderCam.transform.getPosition(), Color(0.1, 0.1, 0.1, 1));

        shader::transformObjToViewSpace(renderCube, renderCam);
        std::clog << ">>>>>>>>>>>>>>>>>to view space:" << std::endl;
        renderCube.mesh.print();

        shader::backFaceCulling(renderCube);
        std::clog << ">>>>>>>>>>>>>>>>>back face culling:" << std::endl;
        renderCube.mesh.print();

        shader::transformObjToPerspProjSpace(renderCube, renderCam);
        std::clog << ">>>>>>>>>>>>>>>>>to persp space:" << std::endl;
        renderCube.mesh.print();

        shader::clipObjectVertices(renderCube);
        shader::applyPerspectiveDivision(renderCube);
        std::clog << ">>>>>>>>>>>>>>>>>apply division:" << std::endl;
        renderCube.mesh.print();

        shader::transformObjToViewportSpace(renderCube, SCREEN_WIDTH, SCREEN_HEIGHT);
        std::clog << ">>>>>>>>>>>>>>>>>to viewport space:" << std::endl;
        renderCube.mesh.print();

        BeginBatchDraw();
        shader::rasterizeObject(renderCube);
        EndBatchDraw();

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
