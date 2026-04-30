#include "scene.h"

namespace Scene {
    Object CreateSceneObject(BasicObjectType type) {
        Object object;
        MeshData& mesh = object.meshData;
        
        switch (type) {
            case BasicObjectType::Cube:
                mesh.vertices = Primitives::CubeVertices();
                mesh.indices = Primitives::CubeIndices();
                break;
        }

        glGenVertexArrays(1, &mesh.VAO);
        glGenBuffers(1, &mesh.VBO);
        glGenBuffers(1, &mesh.EBO);

        glBindVertexArray(mesh.VAO);

        // vertex data
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER,
                    mesh.vertices.size() * sizeof(Vertex),
                    mesh.vertices.data(),
                    GL_STATIC_DRAW);

        // index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    mesh.indices.size() * sizeof(unsigned int),
                    mesh.indices.data(),
                    GL_STATIC_DRAW);

        // xyz
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // no rgb anymore :(
        // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        // glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        return object;
    }
}