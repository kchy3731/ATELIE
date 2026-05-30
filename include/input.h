#pragma once

#include <algorithm>

#include "imgui.h"

#include "state.h"
#include "render.h"

namespace Input {
    void Init(GLFWwindow* window);
    void Process(GLFWwindow* window, AtelieState& state);
}