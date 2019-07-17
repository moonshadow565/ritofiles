cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(ritofiles)
add_executable(ritofiles
  main.cpp
  rito/file.hpp
  rito/types.hpp
  rito/math.hpp
  rito/memory.hpp
  rito/animation.hpp
  rito/animation.cpp
  rito/blend.hpp
  rito/blend.cpp
  rito/simpleskin.hpp
  rito/simpleskin.cpp
  rito/skeleton.hpp
  rito/skeleton.cpp
)
