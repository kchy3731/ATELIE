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

    int total = 0; obj.pivot = glm::vec3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < obj.meshData.vertices.size(); i++) {
        if (obj.cursor != i && !obj.selected[i]) continue;
        obj.pivot += obj.meshData.vertices[i].position;
        total++;
    }
    obj.pivot /= total;

}

void EnterEditMode(AtelieState& state) {
    if (state.scene.size() <= 0) return;
    state.editor.editMode = true;
    Scene::Object& obj = state.scene[state.cursor];
    _UpdateVBO(obj);
}

void LeaveEditMode(AtelieState& state) {
    state.editor.editMode = false;
    Scene::Object& obj = state.scene[state.cursor];
    _UpdateVBO(obj);
}

void ObjectSelect(AtelieState& state) {
    if (!state.multiselect) {
        state.multiselect = true;
        std::fill(state.selected.begin(), state.selected.end(), false);
    }
    state.selected[state.cursor] = !state.selected[state.cursor];
}

void ObjectAdvanceCursor(AtelieState& state, bool forward) {
    if (forward) state.cursor = (state.cursor + 1) % state.selected.size();
    else state.cursor = (state.cursor == 0) ? state.selected.size() - 1 : state.cursor - 1;
}

void EditAdvanceCursor(AtelieState& state, bool forward) {
    Scene::Object& obj = state.scene[state.cursor];
    if (forward) obj.cursor = (obj.cursor + 1) % obj.meshData.vertices.size();
    else obj.cursor = (obj.cursor - 1 == -1) ? obj.meshData.vertices.size() : obj.cursor - 1;
    _UpdateVBO(obj);
}

void EditSelect(AtelieState& state) {
    Scene::Object& obj = state.scene[state.cursor];
    obj.selected[obj.cursor] = !obj.selected[obj.cursor];
    obj.multiselect = true;

    _UpdateVBO(obj);
}

void EditClearSelect(AtelieState& state) {
    Scene::Object& obj = state.scene[state.cursor];
    obj.multiselect = false;
    std::fill(obj.selected.begin(), obj.selected.end(), false);

    _UpdateVBO(obj);
}

void ObjectApply(AtelieState& state) {
    for (int i = 0; i < state.scene.size(); i++) {
        if (i != state.cursor && !state.selected[i]) continue;
        state.scene[i].position += state.editor.previewTranslate;

        glm::vec3 pivot = state.scene[state.cursor].position;
        glm::vec3 offset = state.scene[i].position - pivot;
        glm::quat rotation = state.scene[i].rotation;
        glm::vec3 scale = state.scene[i].scale;
        rotation = state.editor.previewRotate * rotation;
        scale = state.editor.previewScale * scale;
        state.scene[i].position = pivot + state.editor.previewScale * (state.editor.previewRotate * offset);
        state.scene[i].rotation = rotation;
        state.scene[i].scale = scale;
    }
}

void EditApply(AtelieState& state) {
    Scene::Object& obj = state.scene[state.cursor];
    Scene::MeshData& mesh = obj.meshData;
    
    // TODO add center pivot
    for (int i = 0; i < mesh.vertices.size(); i++) {
        if (i != obj.cursor && !obj.selected[i]) continue;
        mesh.vertices[i].position += state.editor.previewTranslate;

        glm::vec3 pivot;
        if (state.editor.constraints.local) pivot = mesh.vertices[obj.cursor].position;
        else pivot = obj.pivot;

        glm::vec3 offset = mesh.vertices[i].position - pivot;
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        rotation = state.editor.previewRotate * rotation;
        scale = state.editor.previewScale * scale;
        mesh.vertices[i].position = pivot + state.editor.previewScale * (state.editor.previewRotate * offset);
    }

    _UpdateVBO(obj);
}