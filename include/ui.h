#pragma once

#include <algorithm>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "state.h"

namespace UI {
    void Init(GLFWwindow* window);

    void Process(AtelieState& state);

    void Cleanup();
}
