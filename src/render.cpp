#include "render.h"

#include <iostream>

#include "primitives.h"
#include "scene.h"

namespace Render {
    
    unsigned int shaderProgram;
    unsigned int wireframeShaderProgram;

    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"

        "uniform mat4 M;\n"
        "uniform mat4 VP;\n"

        "out vec3 vWorldPos;\n"

        "void main() {\n"
        "   vec4 world = M * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   vWorldPos = world.xyz;\n"
        "   gl_Position = VP * world;\n"
        "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
        "in vec3 vWorldPos;\n"
        "out vec4 FragColor;\n"

        "uniform vec3 uCameraPos;\n"

        "void main() {\n"
        "    vec3 N = normalize(cross(dFdx(vWorldPos), dFdy(vWorldPos)));\n"
        "    vec3 V = normalize(uCameraPos - vWorldPos);\n"
        "    float facing = max(dot(N, V), 0.0);\n"
        "    vec3 col = vec3(0.3 + 0.6 * facing);\n"
        "    FragColor = vec4(col, 1.0);\n"
        "}\n\0";

    const char* wireframeVertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 M;\n"
        "uniform mat4 VP;\n"

        "void main() {\n"
        "   vec4 pos = VP * M * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   pos.z -= 0.0001 * pos.w;"
        "   gl_Position = pos;\n"
        "}\n\0";

    const char* wireframeFragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec3 uWireColour;\n"
        
        "void main() {\n"
        "   FragColor = vec4(uWireColour, 1.0);\n"
        "}\n\0";

    int mLocation;
    int vpLocation;
    int uCameraPosLocation;

    int w_mLocation;
    int w_vpLocation;
    int w_uWireColourLocation;

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

        unsigned int wireframeVertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(wireframeVertexShader, 1, &wireframeVertexShaderSource, NULL);
        glCompileShader(wireframeVertexShader);

        unsigned int wireframeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(wireframeFragmentShader, 1, &wireframeFragmentShaderSource, NULL);
        glCompileShader(wireframeFragmentShader);

        wireframeShaderProgram = glCreateProgram();
        glAttachShader(wireframeShaderProgram, wireframeVertexShader);
        glAttachShader(wireframeShaderProgram, wireframeFragmentShader);
        glLinkProgram(wireframeShaderProgram);

        glDeleteShader(wireframeVertexShader);
        glDeleteShader(wireframeFragmentShader);

        mLocation = glGetUniformLocation(shaderProgram, "M");
        vpLocation = glGetUniformLocation(shaderProgram, "VP");
        uCameraPosLocation = glGetUniformLocation(shaderProgram, "uCameraPos");

        w_mLocation = glGetUniformLocation(wireframeShaderProgram, "M");
        w_vpLocation = glGetUniformLocation(wireframeShaderProgram, "VP");
        w_uWireColourLocation = glGetUniformLocation(wireframeShaderProgram, "uWireColour");

    }

    void Draw(const AtelieState& state) {
        glUseProgram(shaderProgram);

        float azRad = glm::radians(state.camera.azimuth);
        float polRad = glm::radians(state.camera.polar);
        glm::vec3 up = glm::vec3(-sin(polRad) * sin(azRad), cos(polRad), -sin(polRad) * cos(azRad));
        glm::mat4 view = glm::lookAt(state.camera.position, glm::vec3(0.0f), up);
        glm::mat4 projection = glm::perspective(glm::radians(40.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glEnable(GL_DEPTH_TEST);

        if (!state.editor.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            for (const auto& obj : state.scene) {
                const Scene::MeshData& mesh = obj.meshData;

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, obj.position);
                model = glm::rotate(model, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::rotate(model, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::scale(model, obj.scale);

                glm::mat4 m = model;
                glm::mat4 vp = projection * view;
                glUniformMatrix4fv(mLocation, 1, GL_FALSE, &m[0][0]);
                glUniformMatrix4fv(vpLocation, 1, GL_FALSE, &vp[0][0]);
                glUniform3fv(uCameraPosLocation, 1, &state.camera.position[0]);

                glBindVertexArray(mesh.VAO);
                glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }
        
        if (state.editor.wireframe) glDisable(GL_DEPTH_TEST);
        // this caused a fun fun fun memleak1!!!!!
        // else { glEnable(GL_POLYGON_OFFSET_LINE); glPolygonOffset(-1.0f, -1.0f); }

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glUseProgram(wireframeShaderProgram);

        for (int i = 0; i < state.scene.size(); i++) {
            const Scene::Object& obj = state.scene[i];
            const Scene::MeshData& mesh = obj.meshData;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj.position);
            model = glm::rotate(model, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::rotate(model, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, obj.scale);

            glm::mat4 m = model;
            glm::mat4 vp = projection * view;
            
            glm::vec3 colour;
            if (i == state.cursor) colour = glm::vec3(1.0f, 0.5f, 0.0f);
            else if (state.selected[i]) colour = glm::vec3(0.65f, 0.35f, 0.08f);
            else colour = glm::vec3(0.0f, 0.0f, 0.0f);

            glUniformMatrix4fv(w_mLocation, 1, GL_FALSE, &m[0][0]);
            glUniformMatrix4fv(w_vpLocation, 1, GL_FALSE, &vp[0][0]);
            glUniform3fv(w_uWireColourLocation, 1, &colour[0]);

            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // if (!state.editor.wireframe) glDisable(GL_POLYGON_OFFSET_LINE);
    }
    
    void Cleanup() {
        glDeleteProgram(shaderProgram);
        glDeleteProgram(wireframeShaderProgram);
    }
}