#include "render.h"  
  
#include <iostream>  
  
#include "primitives.h"  
#include "scene.h"  

namespace Render {  
  
    // --- shaders and uniform (locations)

    struct ProgramUniforms {  
        int m, vp, depthNudge;  
        int previewTranslate, previewRotate, previewScale, pivot;  
        int cameraPos;  
        int wireColour;  
    };  
  
    unsigned int solidShaderProgram;  
    unsigned int wireframeShaderProgram;  
    unsigned int editSolidShaderProgram;  
    unsigned int editWireframeShaderProgram;  
  
    ProgramUniforms solidUniforms;  
    ProgramUniforms wireframeUniforms;  
    ProgramUniforms editSolidUniforms;  
    ProgramUniforms editWireframeUniforms;  
  
    void _CacheUniforms(ProgramUniforms& u, unsigned int program) {  
        u.m                = glGetUniformLocation(program, "M");  
        u.vp               = glGetUniformLocation(program, "VP");  
        u.depthNudge       = glGetUniformLocation(program, "uDepthNudge");  
        u.previewTranslate = glGetUniformLocation(program, "uPreviewTranslate");  
        u.previewRotate    = glGetUniformLocation(program, "uPreviewRotate");  
        u.previewScale     = glGetUniformLocation(program, "uPreviewScale");  
        u.pivot            = glGetUniformLocation(program, "uPivot");  
        u.cameraPos        = glGetUniformLocation(program, "uCameraPos");  
        u.wireColour       = glGetUniformLocation(program, "uWireColour");  
    }  
    
    // --- shader sources

    const char* simpleVertexShaderSource = "#version 330 core\n"  
        "layout (location = 0) in vec3 aPos;\n"  
  
        "uniform mat4 M;\n"  
        "uniform mat4 VP;\n"  
        "uniform float uDepthNudge;\n"  
  
        "out vec3 vWorldPos;\n"  
  
        "void main() {\n"  
        "   vec4 world = M * vec4(aPos, 1.0);\n"  
        "   vWorldPos = world.xyz;\n"  
        "   vec4 pos = VP * world;\n"  
        "   pos.z += uDepthNudge * pos.w;\n"  
        "   gl_Position = pos;\n"  
        "}\0";  
  
    const char* editVertexShaderSource = "#version 330 core\n"  
        "layout (location = 0) in vec3 aPos;\n"  
        "layout (location = 1) in float aHighlight;\n"  
  
        "uniform mat4 M;\n"  
        "uniform mat4 VP;\n"  
        "uniform float uDepthNudge;\n"  
  
        "uniform vec3 uPreviewTranslate;\n"  
        "uniform mat4 uPreviewRotate;\n"  
        "uniform vec3 uPreviewScale;\n"  
        "uniform vec3 uPivot;\n"  
  
        "out vec3 vWorldPos;\n"  
        "out vec3 col;\n"  
        "out float vHighlight;\n"  
  
        "void main() {\n"  
        "   vec3 p = aPos;\n"  
        "   col = vec3(0.0, 0.0, 0.0);\n"  
        "   if (aHighlight > 0.5) {\n"  
        "       if (aHighlight > 1.5) col = vec3(1.0, 0.5, 0.0);\n"  
        "       else col = vec3(0.65, 0.35, 0.08);"  
        "       p = p - uPivot;\n"  
        "       p = (uPreviewRotate * vec4(p, 1.0)).xyz;\n"  
        "       p = p * uPreviewScale;\n"  
        "       p = p + uPivot;\n"  
        "       p = p + uPreviewTranslate;\n"  
        "   }\n"  
        "   vec4 world = M * vec4(p, 1.0);\n"  
        "   vWorldPos = world.xyz;\n"  
        "   vec4 pos = VP * world;\n"  
        "   pos.z += uDepthNudge * pos.w;\n"  
        "   gl_Position = pos;\n"  
        "   vHighlight = aHighlight;\n"  
        "}\0";  
  
    const char* solidFragmentShaderSource = "#version 330 core\n"  
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
  
    const char* wireframeFragmentShaderSource = "#version 330 core\n"  
        "in vec3 col;\n"  
        "out vec4 FragColor;\n"  
        "uniform vec3 uWireColour;\n"  
  
        "void main() {\n"  
        "   FragColor = vec4(uWireColour, 1.0);\n"  
        "}\n\0";  
  
    const char* editWireframeFragmentShaderSource = "#version 330 core\n"  
        "in float vHighlight;\n"  
        "out vec4 FragColor;\n"  
  
        "void main() {\n"  
        "   vec3 col = vec3(0.0, 0.0, 0.0);\n"  
        "   if (vHighlight > 1.5) col = vec3(1.0, 0.5, 0.0);\n"  
        "   else if (vHighlight > 0.5) col = vec3(0.65, 0.35, 0.08);\n"  
        "   FragColor = vec4(col, 1.0);\n"  
        "}\n\0";  
  
    // ---  static VBOs
  
    unsigned int gridVAO, gridVBO;  
    int numGridVertices;  
  
    unsigned int lineVAO, lineVBO;  
      
    unsigned int previewCubeVAO, previewCubeVBO, previewCubeEBO;  
    int numPreviewCubeIndices;  
    unsigned int previewCylinderVAO, previewCylinderVBO, previewCylinderEBO;  
    int numPreviewCylinderIndices;  
  
    // --- math

    glm::mat4 _CalculateVP(const AtelieState& state) {
        float azRad = glm::radians(state.camera.azimuth);  
        float polRad = glm::radians(state.camera.polar);  
        glm::vec3 up = glm::vec3(-sin(polRad) * sin(azRad), cos(polRad), -sin(polRad) * cos(azRad));  
        glm::mat4 view = glm::lookAt(state.camera.position, glm::vec3(0.0f), up);  
        glm::mat4 projection;  

        if (state.camera.orthographic) {  
            projection = glm::ortho(-state.camera.orthographicSize * 800.0f / 600.0f,  
                                    state.camera.orthographicSize * 800.0f / 600.0f,  
                                    -state.camera.orthographicSize,  
                                    state.camera.orthographicSize,  
                                    0.1f,  
                                    100.0f);  
        }  
        else projection = glm::perspective(glm::radians(40.0f), 800.0f / 600.0f, 0.1f, 100.0f);  

        return projection * view;
    }

    // --- object

    void _DrawObjectSolidPass(const AtelieState& state, const glm::mat4& vp) {
        glUseProgram(solidShaderProgram);  
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  

        for (int i = 0; i < state.scene.size(); i++) {  
            const Scene::Object& obj = state.scene[i];  
            const Scene::MeshData& mesh = obj.meshData;  

            glm::mat4 model = glm::mat4(1.0f);  
            glm::vec3 position = obj.position;  
            if ((state.cursor == i || state.selected[i]) && state.editor.tool != ActiveTool::Spawn) {  
                position = position + state.editor.previewTranslate;  
            }  
            glm::vec3 pivot = state.scene[state.cursor].position;  
            glm::vec3 offset = position - pivot;  
            glm::quat rotation = obj.rotation;  
            glm::vec3 scale = obj.scale;  
            if ((state.cursor == i || state.selected[i]) && state.editor.tool != ActiveTool::Spawn) {  
                rotation = state.editor.previewRotate * rotation;  
                scale = state.editor.previewScale * scale;  
                position = pivot + state.editor.previewScale * (state.editor.previewRotate * offset);  
            }  
            model = glm::translate(model, position);  
            model *= glm::mat4_cast(rotation);  
            model = glm::scale(model, scale);  

            glUniformMatrix4fv(solidUniforms.m, 1, GL_FALSE, &model[0][0]);  
            glUniformMatrix4fv(solidUniforms.vp, 1, GL_FALSE, &vp[0][0]);  
            glUniform1f(solidUniforms.depthNudge, 0.0f);  
            glUniform3fv(solidUniforms.cameraPos, 1, &state.camera.position[0]);  

            glBindVertexArray(mesh.VAO);  
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);  
            glBindVertexArray(0);  
        } 
    }

    void _DrawGrid(const glm::mat4& vp) {
        glUseProgram(wireframeShaderProgram);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 id(1.0f);  
        glm::vec3 gridCol(0.2f, 0.2f, 0.2f);  
        glUniformMatrix4fv(wireframeUniforms.m, 1, GL_FALSE, &id[0][0]);  
        glUniformMatrix4fv(wireframeUniforms.vp, 1, GL_FALSE, &vp[0][0]);  
        glUniform3fv(wireframeUniforms.wireColour, 1, &gridCol[0]);  
        glUniform1f(wireframeUniforms.depthNudge, 0.0f);  
  
        glBindVertexArray(gridVAO);  
        glDrawArrays(GL_LINES, 0, numGridVertices);  
        glBindVertexArray(0);  
    }

    void _DrawObjectWireframePass(const AtelieState& state, const glm::mat4& vp) {
        glUseProgram(wireframeShaderProgram);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (int i = 0; i < state.scene.size(); i++) {  
            const Scene::Object& obj = state.scene[i];  
            const Scene::MeshData& mesh = obj.meshData;  
  
            glm::mat4 model = glm::mat4(1.0f);  
            glm::vec3 position = obj.position;  
            if ((state.cursor == i || state.selected[i]) && state.editor.tool != ActiveTool::Spawn) {  
                position = position + state.editor.previewTranslate;  
            }  
            glm::vec3 pivot = state.scene[state.cursor].position;  
            glm::vec3 offset = position - pivot;  
            glm::quat rotation = obj.rotation;  
            glm::vec3 scale = obj.scale;  
            if ((state.cursor == i || state.selected[i]) && state.editor.tool != ActiveTool::Spawn) {  
                rotation = state.editor.previewRotate * rotation;  
                scale = state.editor.previewScale * scale;  
                position = pivot + state.editor.previewScale * (state.editor.previewRotate * offset);  
            }  
            model = glm::translate(model, position);  
            model *= glm::mat4_cast(rotation);  
            model = glm::scale(model, scale);  
              
            glm::vec3 colour;  
            if (i == state.cursor) colour = glm::vec3(1.0f, 0.5f, 0.0f);  
            else if (state.selected[i]) colour = glm::vec3(0.65f, 0.35f, 0.08f);  
            else colour = glm::vec3(0.0f, 0.0f, 0.0f);  
  
            glUniformMatrix4fv(wireframeUniforms.m, 1, GL_FALSE, &model[0][0]);  
            glUniformMatrix4fv(wireframeUniforms.vp, 1, GL_FALSE, &vp[0][0]);  
            glUniform3fv(wireframeUniforms.wireColour, 1, &colour[0]);  
            glUniform1f(wireframeUniforms.depthNudge, -0.0001f);  
  
            glBindVertexArray(mesh.VAO);  
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);  
            glBindVertexArray(0);  
        } 
    }

    void _DrawSpawnPreview(const AtelieState& state, const glm::mat4& vp) {
        glUseProgram(wireframeShaderProgram);

        auto drawCrosshair = [&](glm::vec3 pos) {  
            glm::vec3 axes[3] = { {0.5f, 0, 0}, {0, 0.5f, 0}, {0, 0, 0.5f} };  
            glm::vec3 colors[3] = { {1,0,0}, {0,1,0}, {0,0,1} };  
              
            glm::mat4 idMat(1.0f);  
            glUniformMatrix4fv(wireframeUniforms.m, 1, GL_FALSE, &idMat[0][0]);  
              
            for (int i = 0; i < 3; i++) {  
                glm::vec3 pts[2] = { pos - axes[i], pos + axes[i] };  
                glBindBuffer(GL_ARRAY_BUFFER, lineVBO);  
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pts), pts);  
                glUniform3fv(wireframeUniforms.wireColour, 1, &colors[i][0]);  
                glBindVertexArray(lineVAO);  
                glDrawArrays(GL_LINES, 0, 2);  
            }  
        };  
        
        drawCrosshair(state.editor.previewTranslate);  
        glBindVertexArray(0);  

        if (state.editor.spawnActive) {  
            glm::mat4 model = glm::mat4(1.0f);  
            model = glm::translate(model, state.editor.previewTranslate);  
              
            glUniformMatrix4fv(wireframeUniforms.m, 1, GL_FALSE, &model[0][0]);  
            glUniformMatrix4fv(wireframeUniforms.vp, 1, GL_FALSE, &vp[0][0]);  
            glm::vec3 ghostCol(0.8f, 0.8f, 0.8f);  
            glUniform3fv(wireframeUniforms.wireColour, 1, &ghostCol[0]);  
              
            if (state.editor.spawnType == Scene::BasicObjectType::Cube) {  
                glBindVertexArray(previewCubeVAO);  
                glDrawElements(GL_TRIANGLES, numPreviewCubeIndices, GL_UNSIGNED_INT, 0);  
            } else if (state.editor.spawnType == Scene::BasicObjectType::Cylinder) {  
                glBindVertexArray(previewCylinderVAO);  
                glDrawElements(GL_TRIANGLES, numPreviewCylinderIndices, GL_UNSIGNED_INT, 0);  
            }  
            glBindVertexArray(0);  
        }  
    }

    void _DrawConstraints(const AtelieState& state, const glm::mat4& vp) {
        glUseProgram(wireframeShaderProgram);

        auto drawAxis = [&](bool active, int axisIdx, glm::vec3 colour) {  
            if (!active) return;  
            glm::vec3 dir(0.0f);  
            if (!state.editor.editMode && state.editor.constraints.local) {  
                glm::mat3 anchorRotBasis = GetRotBasis(state.scene[state.cursor]);  
                dir = anchorRotBasis[axisIdx];  
            } else {  
                dir[axisIdx] = 1.0f;  
            }  
            glm::vec3 pivot;
            if (state.editor.editMode) {
                if (state.editor.constraints.local) {
                    // amazing
                    pivot = state.scene[state.cursor].meshData.vertices[state.scene[state.cursor].cursor].position;
                } else {
                    pivot = state.scene[state.cursor].pivot;
                }
            }
            else pivot = state.scene[state.cursor].position;
            glm::vec3 p1 = pivot - dir * 1000.0f;  
            glm::vec3 p2 = pivot + dir * 1000.0f;  
            glm::vec3 pts[2] = {p1, p2};  
                
            glBindBuffer(GL_ARRAY_BUFFER, lineVBO);  
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pts), pts);  
                
            glm::mat4 idMat(1.0f);  
            glUniformMatrix4fv(wireframeUniforms.m, 1, GL_FALSE, &idMat[0][0]);  
            glUniform3fv(wireframeUniforms.wireColour, 1, &colour[0]);  
            glUniform1f(wireframeUniforms.depthNudge, 0.0f);  
            glBindVertexArray(lineVAO);  
            glDrawArrays(GL_LINES, 0, 2);  
        };  

        drawAxis(state.editor.constraints.x, 0, glm::vec3(1.0f, 0.2f, 0.2f));  
        drawAxis(state.editor.constraints.y, 1, glm::vec3(0.2f, 1.0f, 0.2f));  
        drawAxis(state.editor.constraints.z, 2, glm::vec3(0.2f, 0.2f, 1.0f));  
        glBindVertexArray(0); 
    }

    // --- edit

    void _DrawEditSolidPass(const AtelieState& state, const glm::mat4& vp) {
        glUseProgram(solidShaderProgram);  
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  

        for (int i = 0; i < state.scene.size(); i++) {  
            if (state.cursor == i) continue;
            const Scene::Object& obj = state.scene[i];  
            const Scene::MeshData& mesh = obj.meshData;  

            glm::mat4 model = glm::mat4(1.0f);  
            glm::vec3 position = obj.position;  
            glm::quat rotation = obj.rotation;  
            glm::vec3 scale = obj.scale;  
            model = glm::translate(model, position);  
            model *= glm::mat4_cast(rotation);  
            model = glm::scale(model, scale);  

            glUniformMatrix4fv(solidUniforms.m, 1, GL_FALSE, &model[0][0]);  
            glUniformMatrix4fv(solidUniforms.vp, 1, GL_FALSE, &vp[0][0]);  
            glUniform3fv(solidUniforms.cameraPos, 1, &state.camera.position[0]);  

            glBindVertexArray(mesh.VAO);  
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);  
            glBindVertexArray(0);  
        } 
    }

    void _DrawEditBackgroundWireframe(const AtelieState& state, const glm::mat4& vp) {
        glUseProgram(wireframeShaderProgram);  
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   

        for (int i = 0; i < state.scene.size(); i++) {  
            if (i == state.cursor) continue;  
            const Scene::Object& obj = state.scene[i];  
            const Scene::MeshData& mesh = obj.meshData;  

            glm::mat4 model = glm::mat4(1.0f);  
            glm::vec3 position = obj.position;  
            glm::quat rotation = obj.rotation;  
            glm::vec3 scale = obj.scale;  
            model = glm::translate(model, position);  
            model *= glm::mat4_cast(rotation);  
            model = glm::scale(model, scale);  

            glm::vec3 colour = glm::vec3(0.02f, 0.05f, 0.02f);  
            glUniformMatrix4fv(wireframeUniforms.m, 1, GL_FALSE, &model[0][0]);  
            glUniformMatrix4fv(wireframeUniforms.vp, 1, GL_FALSE, &vp[0][0]);  
            glUniform3fv(wireframeUniforms.wireColour, 1, &colour[0]);  

            glBindVertexArray(mesh.VAO);  
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);  
            glBindVertexArray(0);  
        }  
    }

    void _DrawEditActiveSolid(const AtelieState& state, const glm::mat4& vp) {
        glUseProgram(editSolidShaderProgram);  
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  
  
        const Scene::Object& obj = state.scene[state.cursor];  
        const Scene::MeshData& mesh = obj.meshData;  
  
        glm::mat4 model = glm::mat4(1.0f);  
        glm::vec3 position = obj.position;  
        glm::quat rotation = obj.rotation;  
        glm::vec3 scale = obj.scale;  
        model = glm::translate(model, position);  
        model *= glm::mat4_cast(rotation);  
        model = glm::scale(model, scale);
    
        glm::vec3 pivot;
        if (state.editor.constraints.local) pivot = mesh.vertices[obj.cursor].position;
        else pivot = obj.pivot;

        glm::mat4 id = glm::mat4(1.0f);  
        glUniformMatrix4fv(editSolidUniforms.m, 1, GL_FALSE, &model[0][0]);  
        glUniformMatrix4fv(editSolidUniforms.vp, 1, GL_FALSE, &vp[0][0]);  
        glUniform1f(editSolidUniforms.depthNudge, 0.0f);  
        glUniform3fv(editSolidUniforms.previewTranslate, 1, &state.editor.previewTranslate[0]);  
        glm::mat4 previewRotMap = glm::mat4_cast(state.editor.previewRotate);  
        glUniformMatrix4fv(editSolidUniforms.previewRotate, 1, GL_FALSE, &previewRotMap[0][0]);  
        glUniform3fv(editSolidUniforms.previewScale, 1, &state.editor.previewScale[0]);  
        glUniform3fv(editSolidUniforms.pivot, 1, &pivot[0]);  
        glUniform3fv(editSolidUniforms.cameraPos, 1, &state.camera.position[0]);  
  
        glBindVertexArray(mesh.VAO);  
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);  
        glDrawElements(GL_POINTS, mesh.indices.size(), GL_UNSIGNED_INT, 0);  
        glBindVertexArray(0);
    }

    void _DrawEditActiveWireframe(const AtelieState& state, const glm::mat4& vp) {
        glUseProgram(editWireframeShaderProgram);  
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  
        glPointSize(8.0f);  
  
        const Scene::Object& obj = state.scene[state.cursor];  
        const Scene::MeshData& mesh = obj.meshData;  
  
        glm::mat4 model = glm::mat4(1.0f);  
        glm::vec3 position = obj.position;  
        glm::quat rotation = obj.rotation;  
        glm::vec3 scale = obj.scale;  
        model = glm::translate(model, position);  
        model *= glm::mat4_cast(rotation);  
        model = glm::scale(model, scale);  
  
        glm::vec3 pivot;
        if (state.editor.constraints.local) pivot = mesh.vertices[obj.cursor].position;
        else pivot = obj.pivot;

        glm::mat4 id = glm::mat4(1.0f);  
        glUniformMatrix4fv(editWireframeUniforms.m, 1, GL_FALSE, &model[0][0]);  
        glUniformMatrix4fv(editWireframeUniforms.vp, 1, GL_FALSE, &vp[0][0]);  
        glUniform1f(editWireframeUniforms.depthNudge, -0.0001f);  
        glUniform3fv(editWireframeUniforms.previewTranslate, 1, &state.editor.previewTranslate[0]);  
        glm::mat4 previewRotMap = glm::mat4_cast(state.editor.previewRotate);  
        glUniformMatrix4fv(editWireframeUniforms.previewRotate, 1, GL_FALSE, &previewRotMap[0][0]);  
        glUniform3fv(editWireframeUniforms.previewScale, 1, &state.editor.previewScale[0]);  
        glUniform3fv(editWireframeUniforms.pivot, 1, &pivot[0]);  
  
        glBindVertexArray(mesh.VAO);  
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);  
        glDrawElements(GL_POINTS, mesh.indices.size(), GL_UNSIGNED_INT, 0);  
        glBindVertexArray(0);
    }

    // --- mode

    void _DrawObjectMode(const AtelieState& state) {  
        glm::mat4 vp = _CalculateVP(state);
  
        if (!state.editor.wireframe) {  
            glEnable(GL_DEPTH_TEST);  
            _DrawObjectSolidPass(state, vp);
        } else {
            glDisable(GL_DEPTH_TEST);  
        }
          
        _DrawGrid(vp);
        _DrawObjectWireframePass(state, vp);

        if (state.editor.tool == ActiveTool::Spawn) {  
            _DrawSpawnPreview(state, vp);
        }  
  
        if (state.scene.size() > 0 && (state.editor.constraints.x || state.editor.constraints.y || state.editor.constraints.z)) {  
            glDisable(GL_DEPTH_TEST);
            _DrawConstraints(state, vp);
            glEnable(GL_DEPTH_TEST);  
        }  
    }
  
    void _DrawEditMode(const AtelieState& state) {  
        glm::mat4 vp = _CalculateVP(state);
  
        if (!state.editor.wireframe) {  
            glEnable(GL_DEPTH_TEST);  
            _DrawEditSolidPass(state, vp);
            _DrawEditActiveSolid(state, vp); 
        } else {
            glDisable(GL_DEPTH_TEST);  
        } 

        _DrawGrid(vp);
  
        if (state.editor.wireframe) {  
            _DrawEditBackgroundWireframe(state, vp);
        }  
  
        _DrawEditActiveWireframe(state, vp);

        if (state.scene.size() > 0 && (state.editor.constraints.x || state.editor.constraints.y || state.editor.constraints.z)) {  
            glDisable(GL_DEPTH_TEST);
            _DrawConstraints(state, vp);
            glEnable(GL_DEPTH_TEST);  
        }  
    }

    // --- public
  
    void Init() {  
        unsigned int simpleVertexShader = glCreateShader(GL_VERTEX_SHADER);  
        glShaderSource(simpleVertexShader, 1, &simpleVertexShaderSource, NULL);  
        glCompileShader(simpleVertexShader);  
  
        unsigned int editVertexShader = glCreateShader(GL_VERTEX_SHADER);  
        glShaderSource(editVertexShader, 1, &editVertexShaderSource, NULL);  
        glCompileShader(editVertexShader);  
  
        unsigned int solidFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  
        glShaderSource(solidFragmentShader, 1, &solidFragmentShaderSource, NULL);  
        glCompileShader(solidFragmentShader);  
  
        unsigned int wireframeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  
        glShaderSource(wireframeFragmentShader, 1, &wireframeFragmentShaderSource, NULL);  
        glCompileShader(wireframeFragmentShader);  
  
        unsigned int editWireframeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  
        glShaderSource(editWireframeFragmentShader, 1, &editWireframeFragmentShaderSource, NULL);  
        glCompileShader(editWireframeFragmentShader);  
  
        solidShaderProgram = glCreateProgram();  
        glAttachShader(solidShaderProgram, simpleVertexShader);  
        glAttachShader(solidShaderProgram, solidFragmentShader);  
        glLinkProgram(solidShaderProgram);  
  
        wireframeShaderProgram = glCreateProgram();  
        glAttachShader(wireframeShaderProgram, simpleVertexShader);  
        glAttachShader(wireframeShaderProgram, wireframeFragmentShader);  
        glLinkProgram(wireframeShaderProgram);  
  
        editSolidShaderProgram = glCreateProgram();  
        glAttachShader(editSolidShaderProgram, editVertexShader);  
        glAttachShader(editSolidShaderProgram, solidFragmentShader);  
        glLinkProgram(editSolidShaderProgram);  
  
        editWireframeShaderProgram = glCreateProgram();  
        glAttachShader(editWireframeShaderProgram, editVertexShader);  
        glAttachShader(editWireframeShaderProgram, editWireframeFragmentShader);  
        glLinkProgram(editWireframeShaderProgram);  
  
        glDeleteShader(simpleVertexShader);  
        glDeleteShader(editVertexShader);  
        glDeleteShader(solidFragmentShader);  
        glDeleteShader(wireframeFragmentShader);  
        glDeleteShader(editWireframeFragmentShader);  
  
        _CacheUniforms(solidUniforms, solidShaderProgram);  
        _CacheUniforms(wireframeUniforms, wireframeShaderProgram);  
        _CacheUniforms(editSolidUniforms, editSolidShaderProgram);  
        _CacheUniforms(editWireframeUniforms, editWireframeShaderProgram);  
  
        // ---  
  
        std::vector<glm::vec3> gridVerts;  
        float size = 50.0f;  
        float step = 0.5f;  
        for (float i = -size; i <= size; i += step) {  
            gridVerts.push_back(glm::vec3(i, 0.0f, -size));  
            gridVerts.push_back(glm::vec3(i, 0.0f, size));  
            gridVerts.push_back(glm::vec3(-size, 0.0f, i));  
            gridVerts.push_back(glm::vec3(size, 0.0f, i));  
        }  
        numGridVertices = gridVerts.size();  
  
        glGenVertexArrays(1, &gridVAO);  
        glGenBuffers(1, &gridVBO);  
        glBindVertexArray(gridVAO);  
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);  
        glBufferData(GL_ARRAY_BUFFER, gridVerts.size() * sizeof(glm::vec3), gridVerts.data(), GL_STATIC_DRAW);  
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);  
        glEnableVertexAttribArray(0);  
        glBindVertexArray(0);  
  
        glGenVertexArrays(1, &lineVAO);  
        glGenBuffers(1, &lineVBO);  
        glBindVertexArray(lineVAO);  
        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);  
        glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);  
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);  
        glEnableVertexAttribArray(0);  
        glBindVertexArray(0);  
  
        std::vector<Vertex> cv = Primitives::CubeVertices();  
        std::vector<unsigned int> ci = Primitives::CubeIndices();  
        numPreviewCubeIndices = ci.size();  
        glGenVertexArrays(1, &previewCubeVAO);  
        glGenBuffers(1, &previewCubeVBO);  
        glGenBuffers(1, &previewCubeEBO);  
        glBindVertexArray(previewCubeVAO);  
        glBindBuffer(GL_ARRAY_BUFFER, previewCubeVBO);  
        glBufferData(GL_ARRAY_BUFFER, cv.size() * sizeof(Vertex), cv.data(), GL_STATIC_DRAW);  
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previewCubeEBO);  
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ci.size() * sizeof(unsigned int), ci.data(), GL_STATIC_DRAW);  
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);  
        glEnableVertexAttribArray(0);  
  
        std::vector<Vertex> cyv = Primitives::CylinderVertices();  
        std::vector<unsigned int> cyi = Primitives::CylinderIndices();  
        numPreviewCylinderIndices = cyi.size();  
        glGenVertexArrays(1, &previewCylinderVAO);  
        glGenBuffers(1, &previewCylinderVBO);  
        glGenBuffers(1, &previewCylinderEBO);  
        glBindVertexArray(previewCylinderVAO);  
        glBindBuffer(GL_ARRAY_BUFFER, previewCylinderVBO);  
        glBufferData(GL_ARRAY_BUFFER, cyv.size() * sizeof(Vertex), cyv.data(), GL_STATIC_DRAW);  
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, previewCylinderEBO);  
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, cyi.size() * sizeof(unsigned int), cyi.data(), GL_STATIC_DRAW);  
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);  
        glEnableVertexAttribArray(0);  
  
        glBindVertexArray(0);  
    }

    void Draw(const AtelieState& state) {  
        if (state.editor.editMode) _DrawEditMode(state);  
        else _DrawObjectMode(state);  
    }
      
    void Cleanup() {  
        glDeleteProgram(solidShaderProgram);  
        glDeleteProgram(wireframeShaderProgram);  
        glDeleteProgram(editSolidShaderProgram);  
        glDeleteProgram(editWireframeShaderProgram);  
    }
}  