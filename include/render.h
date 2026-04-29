#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "state.h"

namespace Render {
    void Init();
    void Draw(const AtelieState& state);
    int PickObject(const AtelieState& state, double x, double y);
    void Cleanup();
}