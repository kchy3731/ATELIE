#include "state.h"

glm::mat3 GetRotBasis(const Scene::Object& obj) {
    return glm::mat3_cast(obj.rotation);
}

glm::vec3 GetCameraRight(const AtelieState& state) {
    float az = glm::radians(state.camera.azimuth);
    return glm::vec3(cos(az), 0.0f, -sin(az));
}

glm::vec3 GetCameraFront(const AtelieState& state) {
    float az = glm::radians(state.camera.azimuth);
    float pol = glm::radians(state.camera.polar);
    return glm::vec3(-cos(pol) * sin(az), -sin(pol), -cos(pol) * cos(az));
}

glm::vec3 GetCameraUp(const AtelieState& state) {
    float az = glm::radians(state.camera.azimuth);
    float pol = glm::radians(state.camera.polar);
    return glm::vec3(-sin(pol) * sin(az), cos(pol), -sin(pol) * cos(az));
}

void EnterEditMode(AtelieState& state) {
    state.editor.editMode = true;
}

void LeaveEditMode(AtelieState& state) {
    state.editor.editMode = false;
}

void _UpdateVBO(Scene::Object& obj) {
    Scene::MeshData& mesh = obj.meshData;
    for (int i = 0; i < mesh.vertices.size(); i++) {
        if (i == obj.cursor) mesh.vertices[i].highlight = 2.0f; 
        else if (obj.selected[i]) mesh.vertices[i].highlight = 1.0f;
        else mesh.vertices[i].highlight = 0.0f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    mesh.vertices.size() * sizeof(Vertex),
                    mesh.vertices.data()); 
}

void EditAdvanceCursor(AtelieState& state) {
    Scene::Object& obj = state.scene[state.cursor];
    obj.cursor = (obj.cursor + 1) % obj.meshData.vertices.size();
    _UpdateVBO(obj);
}

void EditSelect(AtelieState& state) {
    Scene::Object& obj = state.scene[state.cursor];
    obj.selected[obj.cursor] = true;
}