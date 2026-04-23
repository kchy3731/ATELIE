#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void ui_init(GLFWwindow* window);

void ui_process();

void ui_cleanup();

