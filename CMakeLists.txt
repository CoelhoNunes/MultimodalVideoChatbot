cmake_minimum_required(VERSION 3.16)
project(MultimodalVideoChatbot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_CUDA "Enable ONNX Runtime GPU support" OFF)
option(BUILD_GUI "Build Qt GUI" OFF)
option(BUILD_TESTS "Enable Google Test" ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Multimedia Network WebSockets)
find_package(FFmpeg REQUIRED)
find_package(ONNXRuntime REQUIRED)
find_package(faiss REQUIRED)
find_package(fastText REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio)

add_subdirectory(src)

if (BUILD_TESTS)
  enable_testing()
  find_package(GTest REQUIRED)
  add_subdirectory(tests)
endif()