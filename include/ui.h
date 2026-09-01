#pragma once

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "state.h"

namespace UI {
    void Init(GLFWwindow* window);

    void Process(AtelieState& state);

    void Cleanup();
}
