cmake_minimum_required(VERSION 3.10)
project(ATELIE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(ATELIE 
    src/main.cpp 
    src/ui.cpp
    # glad
    lib/glad/glad.c
    # imgui
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(ATELIE PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(ATELIE PRIVATE ${CMAKE_SOURCE_DIR}/lib/imgui)
target_include_directories(ATELIE PRIVATE ${CMAKE_SOURCE_DIR}/lib/imgui/backends)

target_link_libraries(ATELIE
    PRIVATE
    OpenGL::GL
    glfw
    dl
    m
)
