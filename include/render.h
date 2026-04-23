#pragma once

#include "state.h"

namespace Render {
    void Init();
    void Draw(const AtelieState& state);
    void Cleanup();
}