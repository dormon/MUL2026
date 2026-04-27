cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

project(MUL2026)

find_package(OpenCV REQUIRED)

option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON )
add_subdirectory(libs/SDL-release-3.4.4)
add_subdirectory(libs/geGL)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 geGL::geGL ${OpenCV_LIBS} opencv_highgui)

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

add_custom_target(run ./${PROJECT_NAME})
