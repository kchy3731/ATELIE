cmake_minimum_required(VERSION 3.16)
project(ATELIE LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenGL REQUIRED)
add_subdirectory(vendor)

add_executable(ATELIE
    src/main.cpp src/state.cpp src/input.cpp src/render.cpp
    src/primitives.cpp src/scene.cpp src/ui.cpp
)
target_include_directories(ATELIE PRIVATE include)
target_link_libraries(ATELIE PRIVATE OpenGL::GL glfw glm glad imgui)