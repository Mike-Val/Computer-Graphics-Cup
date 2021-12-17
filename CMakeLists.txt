cmake_minimum_required(VERSION 3.21)
project(Computer_Graphics_Cup)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(Computer_Graphics_Cup
        BoundingBox.hpp
        Cone.hpp
        Hit.hpp
        Image.h
        Light.hpp
        main.cpp
        Material.h
        OBJ.hpp
        Object.hpp
        Plane.hpp
        Ray.hpp
        Scene.hpp
        Sphere.hpp
        Textures.h
        thread_pool.hpp
        Triangle.hpp
        Utils.hpp)
