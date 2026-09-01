#pragma once

#include <imgui/imgui.h>

#include "state.h"

namespace Input {
    void Init(GLFWwindow* window);
    void Process(GLFWwindow* window, AtelieState& state);
}