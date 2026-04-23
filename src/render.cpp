#include "render.h"
#include <glad/glad.h>
#include <iostream>

namespace Render {
    
    unsigned int shaderProgram;
    unsigned int VAO, VBO;

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

        float vertices[] = {
            // xyz              // rgb
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
             0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // xyz
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // rgb
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void Draw(const AtelieState& state) {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // model view projection
        glm::mat4 model = glm::mat4(1.0f); // identity matrix
        glm::mat4 view = glm::lookAt(state.camera.position, state.camera.position + state.camera.front, state.camera.up);
        glm::mat4 projection = glm::perspective(glm::radians(state.camera.fov), 640.0f / 480.0f, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        int matrixLocation = glGetUniformLocation(shaderProgram, "MVP");
        glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &mvp[0][0]);

        if (state.showWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }
    
    void Cleanup() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
}