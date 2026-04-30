#include "primitives.h"

namespace Primitives {
    std::vector<Vertex> CubeVertices() {
        return {
            { glm::vec3(-0.5f, -0.5f,  0.5f) },
            { glm::vec3( 0.5f, -0.5f,  0.5f) },
            { glm::vec3( 0.5f,  0.5f,  0.5f) },
            { glm::vec3(-0.5f,  0.5f,  0.5f) },
            { glm::vec3(-0.5f, -0.5f, -0.5f) },
            { glm::vec3( 0.5f, -0.5f, -0.5f) },
            { glm::vec3( 0.5f,  0.5f, -0.5f) },
            { glm::vec3(-0.5f,  0.5f, -0.5f) } 
        };
    }

    std::vector<unsigned int> CubeIndices() {
        return {
            0, 1, 2, 2, 3, 0,
            1, 5, 6, 6, 2, 1,
            7, 6, 5, 5, 4, 7,
            4, 0, 3, 3, 7, 4,
            3, 2, 6, 6, 7, 3,
            4, 5, 1, 1, 0, 4 
        };
    }

    std::vector<Vertex> TriangleVertices() {
        return {
            { glm::vec3( 0.5f, -0.5f, 0.0f) },
            { glm::vec3(-0.5f, -0.5f, 0.0f) },
            { glm::vec3( 0.0f,  0.5f, 0.0f) }
        };
    }

    std::vector<unsigned int> TriangleIndices() {
        return { 0, 1, 2 };
    }
}