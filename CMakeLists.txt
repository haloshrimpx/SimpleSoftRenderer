cmake_minimum_required(VERSION 4.0)
project(SoftRenderer)

set(CMAKE_CXX_STANDARD 20)

add_executable(SoftRenderer main.cpp
        src/Vector.cpp
        includes/Vector.h
        src/Matrix.cpp
        includes/Matrix.h
        src/Transform.cpp
        includes/Transform.h
        src/Object.cpp
        includes/Object.h
        src/Mesh.cpp
        includes/Mesh.h
        src/Vertex.cpp
        includes/Vertex.h
        src/Triangle.cpp
        includes/Triangle.h
        src/Rectangle.cpp
        includes/Rectangle.h
        src/Color.cpp
        includes/Color.h
        src/shader_transform.cpp
        includes/shader_transform.h
        src/Camera.cpp
        includes/Camera.h
        src/Light.cpp
        includes/Light.h
        src/shader_geometry.cpp
        includes/shader_geometry.h
        src/shader_render.cpp
        includes/shader_render.h
)

target_link_libraries(SoftRenderer libeasyx.a)

