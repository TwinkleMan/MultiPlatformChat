cmake_minimum_required(VERSION 2.8)

if (CMAKE_VERSION VERSION_LESS "3.1")
    add_definitions(-std=c++17)
else()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS_OFF)
endif()

project(MultiplatformChat)

#library
add_subdirectory(MultiPlatformChat)
include_directories(MultiPlatformChat/src)

#create app
add_executable(Chat MultiPlatformChat/src/main.cpp)

#link engine
target_link_libraries(Chat Engine)
