#pragma once

#include <glad/glad.h>

#include <vector>
#include <glm.hpp>

#include "primitives.h"

namespace Scene {
    enum class BasicObjectType { Cube };

    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO, VBO, EBO;
        bool dirty = false;
    };

    struct Object {
        MeshData meshData;

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        int cursor = 0;
        std::vector<bool> selected;
    };

    Object CreateSceneObject(BasicObjectType type);
}

