#pragma once

#include <vector>
#include <glm.hpp>

struct Vertex {
    glm::vec3 position;
};

namespace Primitives {
    std::vector<Vertex> CubeVertices();
    std::vector<unsigned int> CubeIndices();

    std::vector<Vertex> TriangleVertices();
    std::vector<unsigned int> TriangleIndices();
}