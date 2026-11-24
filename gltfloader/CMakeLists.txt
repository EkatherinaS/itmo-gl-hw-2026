cmake_minimum_required(VERSION 3.16)

project(gltfloader LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(gltfloader SHARED
  gltfloader_global.h
  gltfloader.h
  gltfloader.cpp
  gltfmodel.h
  gltfmodel.cpp
)

target_include_directories(gltfloader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/tinygltf
)

add_subdirectory(tinygltf)
target_link_libraries(gltfloader PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGLWidgets tinygltf)

target_compile_definitions(gltfloader PRIVATE GLTFLOADER_LIBRARY)
