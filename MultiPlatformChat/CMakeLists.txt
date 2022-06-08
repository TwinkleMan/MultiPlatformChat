cmake_minimum_required(VERSION 2.8)

project(Engine)

set(SOURCES
    src/NetworkUnit.cpp
    src/Server.cpp
    src/Client.cpp
    src/NetworkUnit.h
    src/Server.h
    src/Client.h)

include_directories(src)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

if (UNIX)
    target_link_libraries(Engine pthread)
endif()