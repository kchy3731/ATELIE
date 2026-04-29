#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

namespace Primitives {
    std::vector<Vertex> CreateCubeVertices();
    std::vector<unsigned int> CreateCubeIndices();
}