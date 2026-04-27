cmake_minimum_required(VERSION 3.16)

project(MUL2026)

add_executable(${PROJECT_NAME} main.cpp)

add_custom_target(run ./${PROJECT_NAME})
