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


    Transform cameraWorldTransform(
        {0, 0, 3},
        {0, 0, 0}, // 左手坐标系中，绕y+轴顺时针旋转为正方向
        {1, 1, 1}
    );

    Camera camera(60, 0.5, 100, SCREEN_WIDTH, SCREEN_HEIGHT, cameraWorldTransform);

    // geom::Mesh mesh;
    // // 读取文件
    // if (objreader::tryReadObjFile("SphereSmooth.obj", mesh))
    //     std::clog << "READ SUCCESS" << std::endl;
    // else
    //     return 1;

    // mesh.print();

    Material sphereMat(COLOR_WHITE, COLOR_BLACK, 1000);
    Material susannaMat(COLOR_WHITE, COLOR_BLACK, 10);
    Object sphere("sphere", "SphereSmooth.obj",
                  {
                      {2, 0, -3},
                      {0, 0, 0},
                      {1, 1, 1}
                  }, sphereMat);
    Object susanna("susanna", "Susanna.obj", {
                       {0, 0, 0},
                       {20, 0, 20},
                       {1, 1, 1}
                   }, susannaMat);
    Object plane("plane", "Plane.obj", {
                     {0, -2, -3},
                     {0, 0, 0},
                     {1, 1, 1}
                 }, Material(COLOR_WHITE * 0.4, COLOR_BLACK, 10)
    );
    DirectionalLight sun(
        COLOR_WHITE * 0.1, 0.1, {
            {0, 0, 0}, {0, 0, -90}, {1, 1, 1}
        }
    );
    PointLight point_light1(
        1, Color(255, 203, 61).toLinearColor(), 1, {
            {-1, 0, 1},
            {0, 0, 0},
            {1, 1, 1}
        }
    );
    PointLight point_light2(
        1, Color(11, 167, 255).toLinearColor(), 1, {
            {1, 0, 1},
            {0, 0, 0},
            {1, 1, 1}
        }
    );
    PointLight point_light3(
        3, COLOR_RED, 1, {
            {2, 2, -1},
            {0, 0, 0},
            {1, 1, 1}
        }
    );

    std::vector<std::unique_ptr<Light> > lights;
    lights.push_back(std::make_unique<DirectionalLight>(sun));
    lights.push_back(std::make_unique<PointLight>(point_light1));
    lights.push_back(std::make_unique<PointLight>(point_light2));

    ScreenBuffer renderBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

    // 开始渲染
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);

    while (true) {
        Camera renderCam(60, 0.01, 100, SCREEN_WIDTH, SCREEN_HEIGHT, cameraWorldTransform);

        shader::gouraudShadingPipeline({susanna, sphere, plane}, lights, renderCam, renderBuffer);

        const maths::Vector3 &camPos = cameraWorldTransform.getPosition();
        const maths::Vector3 &camRot = cameraWorldTransform.getRotation();

        std::cout << "\n cam position: " << camPos.x << "\t" << camPos.y << "\t" << camPos.z << std::endl;
        std::cout << "\n cam rotation: " << camRot.x << "\t" << camRot.y << "\t" << camRot.z << std::endl;

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
