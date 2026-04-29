#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "state.h"

namespace Render {
    void Init();
    void Draw(const AtelieState& state);
    void Cleanup();
}