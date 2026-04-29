#pragma once

#include <GLFW/glfw3.h>

#include "state.h"

namespace Input {
    void Init(GLFWwindow* window);
    void Process(GLFWwindow* window, AtelieState& state);
}