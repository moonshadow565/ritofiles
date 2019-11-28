cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ritofiles)
add_subdirectory(assimp)

add_executable(ritofiles
    src/main.cpp
    src/rito2assimp.hpp
    src/rito2assimp.cpp
    src/rito/types.hpp
    src/rito/file.hpp
    src/rito/file.cpp
    src/rito/mapgeo.hpp
    src/rito/mapgeo.cpp
    src/rito/animation.hpp
    src/rito/animation.cpp
    src/rito/blend.hpp
    src/rito/blend.cpp
    src/rito/simpleskin.hpp
    src/rito/simpleskin.cpp
    src/rito/skeleton.hpp
    src/rito/skeleton.cpp
)
target_include_directories(ritofiles PUBLIC src)
target_link_libraries(ritofiles assimp)
