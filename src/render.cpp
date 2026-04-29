#include "render.h"
#include <glad/glad.h>
#include <iostream>
#include <unordered_map>
#include "primitives.h"

namespace Render {
    
    unsigned int shaderProgram;

    unsigned int pickingShaderProgram;
    unsigned int pickingFBO;
    unsigned int pickingTexture;
    unsigned int pickingDepth;

    struct MeshData {
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        int indexCount;
    };
    std::unordered_map<MeshType, MeshData> meshes;

    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColour;\n"
        "out vec3 ourColour;\n"
        "uniform mat4 MVP;\n"
        "void main() {\n"
        "   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   ourColour = aColour;\n"
        "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColour;\n"
        "void main() {\n"
        "   FragColor = vec4(ourColour, 1.0f);\n"
        "}\n\0";

    const char* pickingVertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 MVP;\n"
        "void main() {\n"
        "   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    const char* pickingFragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 objectID;\n"
        "void main() {\n"
        "   FragColor = objectID;\n"
        "}\n\0";

    void LoadMesh(MeshType type, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
        MeshData data;
        data.indexCount = indices.size();

        glGenVertexArrays(1, &data.VAO);
        glGenBuffers(1, &data.VBO);
        glGenBuffers(1, &data.EBO);

        glBindVertexArray(data.VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // xyz
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);
        
        // rgb
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        meshes[type] = data;
    }

    void Init() {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Picking FBO Setup
        unsigned int pVertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(pVertexShader, 1, &pickingVertexShaderSource, NULL);
        glCompileShader(pVertexShader);

        unsigned int pFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(pFragmentShader, 1, &pickingFragmentShaderSource, NULL);
        glCompileShader(pFragmentShader);

        pickingShaderProgram = glCreateProgram();
        glAttachShader(pickingShaderProgram, pVertexShader);
        glAttachShader(pickingShaderProgram, pFragmentShader);
        glLinkProgram(pickingShaderProgram);

        glDeleteShader(pVertexShader);
        glDeleteShader(pFragmentShader);

        glGenFramebuffers(1, &pickingFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);

        glGenTextures(1, &pickingTexture);
        glBindTexture(GL_TEXTURE_2D, pickingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTexture, 0);

        glGenRenderbuffers(1, &pickingDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, pickingDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickingDepth);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Load Primitives

        LoadMesh(MeshType::Cube, Primitives::CubeVertices(), Primitives::CubeIndices());
        LoadMesh(MeshType::Triangle, Primitives::TriangleVertices(), Primitives::TriangleIndices());
    }

    int PickObject(const AtelieState& state, double x, double y) {
        glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(pickingShaderProgram);
        
        float azRad = glm::radians(state.camera.azimuth);
        float polRad = glm::radians(state.camera.polar);
        glm::vec3 up = glm::vec3(-sin(polRad) * sin(azRad), cos(polRad), -sin(polRad) * cos(azRad));
        glm::mat4 view = glm::lookAt(state.camera.position, glm::vec3(0.0f), up);
        glm::mat4 projection = glm::perspective(glm::radians(40.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        int matrixLocation = glGetUniformLocation(pickingShaderProgram, "MVP");
        int objectIdLocation = glGetUniformLocation(pickingShaderProgram, "objectID");

        for (int i = 0; i < state.scene.size(); ++i) {
            const auto& obj = state.scene[i];
            auto it = meshes.find(obj.meshType);
            if (it == meshes.end()) continue;
            const MeshData& mesh = it->second;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj.position);
            model = glm::rotate(model, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::rotate(model, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, obj.scale);

            glm::mat4 mvp = projection * view * model;
            glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &mvp[0][0]);

            int id = i + 1;
            float r = (id & 0xFF) / 255.0f;
            float g = ((id >> 8) & 0xFF) / 255.0f;
            float b = ((id >> 16) & 0xFF) / 255.0f;
            glUniform4f(objectIdLocation, r, g, b, 1.0f);

            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        }

        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        unsigned char data[4];
        glReadPixels((int)x, 600 - (int)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (data[0] == 255 && data[1] == 255 && data[2] == 255) {
            return -1;
        }

        int pickedID = data[0] + (data[1] << 8) + (data[2] << 16);
        return pickedID - 1;
    }

    void Draw(const AtelieState& state) {
        glUseProgram(shaderProgram);

        float azRad = glm::radians(state.camera.azimuth);
        float polRad = glm::radians(state.camera.polar);
        glm::vec3 up = glm::vec3(-sin(polRad) * sin(azRad), cos(polRad), -sin(polRad) * cos(azRad));
        glm::mat4 view = glm::lookAt(state.camera.position, glm::vec3(0.0f), up);
        glm::mat4 projection = glm::perspective(glm::radians(40.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        int matrixLocation = glGetUniformLocation(shaderProgram, "MVP");

        if (state.showWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        for (const auto& obj : state.scene) {
            auto it = meshes.find(obj.meshType);
            if (it == meshes.end()) continue;
            const MeshData& mesh = it->second;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj.position);
            model = glm::rotate(model, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::rotate(model, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, obj.scale);

            glm::mat4 mvp = projection * view * model;
            glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &mvp[0][0]);

            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);
    }
    
    void Cleanup() {
        for (auto& pair : meshes) {
            glDeleteVertexArrays(1, &pair.second.VAO);
            glDeleteBuffers(1, &pair.second.VBO);
            glDeleteBuffers(1, &pair.second.EBO);
        }
        meshes.clear();
        glDeleteProgram(shaderProgram);
        glDeleteProgram(pickingShaderProgram);
        glDeleteFramebuffers(1, &pickingFBO);
        glDeleteTextures(1, &pickingTexture);
        glDeleteRenderbuffers(1, &pickingDepth);
    }
}