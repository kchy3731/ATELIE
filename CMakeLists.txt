cmake_minimum_required(VERSION 3.10)
project(ATELIE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(ATELIE 
    src/main.cpp 
    src/glad.c
)

target_include_directories(ATELIE PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(ATELIE
    PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    dl
    m
)
