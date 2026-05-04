#pragma once

#include <glad/glad.h>

#include <vector>
#include <glm.hpp>
#include <gtc/quaternion.hpp>

#include "primitives.h"

namespace Scene {
    enum class BasicObjectType { Cube, Cylinder };

    struct MeshData {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        unsigned int VAO, VBO, EBO;
        bool dirty = false;
    };

    struct Object {
        MeshData meshData;

        glm::vec3 pivot = glm::vec3(0.0f);
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        int cursor = 0;
        std::vector<bool> selected;
        bool multiselect = false;
    };

    Object CreateSceneObject(BasicObjectType type);
    void DestroySceneObject(Object& object);
}

