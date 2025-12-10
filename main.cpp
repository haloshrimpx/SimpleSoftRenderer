#include <complex>
#include <easyx.h>
#include <iostream>
#include <vector>
#include <conio.h>

#include "includes/Buffer.h"
#include "includes/obj_reader.h"
#include "includes/Camera.h"
#include "includes/Light.h"
#include "includes/Matrix.h"
#include "includes/Object.h"
#include "includes/shader_geometry.h"
#include "includes/shader_render.h"
#include "includes/shader_transform.h"
#include "includes/Transform.h"

int awfwa(int argc, char *argv[]) {
    maths::Vector4 worldPos{2, 0, -3, 1}; // 世界空间

    Transform cameraWorldTransform(
        {0, 0, 3},
        {0, 0, 0}, // 左手坐标系中，绕y+轴顺时针旋转为正方向
        {1, 1, 1}
    );

    Camera camera(60, 0.5, 100, 800, 600, cameraWorldTransform);

    Object obj("s", "triangle.obj", {{0, 0, 0}, {0, 0, 0}, {1, 1, 1}}, Material());

    shader::transformObjToWorldSpace(obj);

    obj.mesh.triangles[0].vertColors[0] = COLOR_BLUE;
    obj.mesh.triangles[0].vertColors[1] = COLOR_RED;
    obj.mesh.triangles[0].vertColors[2] = COLOR_GREEN;

    maths::Matrix4x4 vp = shader::getPerspVPMatrix(camera, camera.transform);

    for (auto &vertex: obj.mesh.vertices) {
        vertex.pos = maths::Matrix4x4::multiply(vp, vertex.pos);
    }

    shader::clipMesh(obj.mesh);

    std::clog << ">>>>>>>>>>>MESH CLIPPING" << std::endl;
    obj.mesh.print();

    shader::applyPerspectiveDivision(obj);

    std::clog << ">>>>>>>>>>>PERSPECTIVE DIVISION" << std::endl;
    obj.mesh.print();

    shader::transformObjToViewportSpace(obj, camera.windowWidth, camera.windowHeight);

    std::clog << ">>>>>>>>>>>VIEWPORT SPACE" << std::endl;
    obj.mesh.print();


    ScreenBuffer buffer(800, 600);
    buffer.clear();
    initgraph(800, 600);

    shader::rasterizeObject(obj, buffer, maths::Matrix4x4(), maths::Matrix4x4(), DepthBuffer(800, 600));
    shader::renderBuffer(buffer);

    system("pause");

    // // DEBUG 点光源的阴影
    // DepthBuffer depthBuffer(1024, 1024);
    // Camera lightCamera(60, 0.05, 100, 1024, 1024, {
    //                        {0, 4, 4}, {-60, 0, 0}, {1, 1, 1}
    //                    }
    // );
    // maths::Matrix4x4 vp = shader::getPerspVPMatrix(camera, camera.transform);

    // worldPos = maths::Matrix4x4::multiply(vp, worldPos);
    // std::clog << " vp transformation, clip space" << std::endl;
    // worldPos.print();
    //
    // worldPos = shader::perspectiveDivision(worldPos);
    // std::clog << "pos transformation" << std::endl;
    // worldPos.print();
    //
    // worldPos = shader::viewportTransformation(worldPos, 800, 600);
    // std::clog << "viewport " << std::endl;
    // worldPos.print(); // 屏幕空间
    //
    // maths::Matrix4x4 invVPMatr = vp.inverse();
    //
    // // 变换回世界空间
    // worldPos = shader::screenPosToWorld(worldPos.toVector3(), invVPMatr, 800, 600, camera.zoomNear, camera.zoomFar);
    // std::clog << "screenPosToWorld " << std::endl;
    // worldPos.print();
    //
    // // 看看变换到点光源的屏幕空间是不是一样的值
    // maths::Vector4 worldPos_Light{2, 0, -1, 1};
    // maths::Matrix4x4 lightVPMatr = shader::getPerspVPMatrix(lightCamera, lightCamera.transform);
    //
    // worldPos_Light = maths::Matrix4x4::multiply(lightVPMatr, worldPos_Light);
    //
    // worldPos_Light = shader::perspectiveDivision(worldPos_Light);
    //
    // worldPos_Light = shader::viewportTransformation(worldPos_Light, 1024, 1024);
    //
    // std::clog << "screenPosToWorld_Light :" << std::endl;
    // worldPos_Light.print();
    //

    // 相机->世界空间->光源屏幕空间
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
            {0, 3, 0},
            {0, 0, -90},
            {1, 1, 1}
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
            {0, 4, 4},
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
        Camera renderCam(60, 0.05, 1000, SCREEN_WIDTH, SCREEN_HEIGHT, cameraWorldTransform);

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
