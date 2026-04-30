cmake_minimum_required(VERSION 3.10)
project(ATELIE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(ATELIE 
    src/main.cpp 
    src/input.cpp
    src/render.cpp
    src/primitives.cpp
    src/scene.cpp
    src/ui.cpp

    # glad
    vendor/glad/src/glad.c

    # imgui
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(ATELIE PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(ATELIE PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glm)
target_include_directories(ATELIE PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glad/include)
target_include_directories(ATELIE PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui)
target_include_directories(ATELIE PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui/backends)

target_link_libraries(ATELIE
    PRIVATE
    OpenGL::GL
    glfw
    dl
    m
)
