#include "input.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace Input {
    // private
    void _UpdateCameraPosition(AtelieState& state) {
        state.camera.polar = (state.camera.polar > 90.0f) ? 90.0f : state.camera.polar;
        state.camera.polar = (state.camera.polar < -90.0f) ? -90.0f : state.camera.polar;
        float azRad = glm::radians(state.camera.azimuth);
        float polRad = glm::radians(state.camera.polar);

        state.camera.position.x = state.camera.radius * cos(polRad) * sin(azRad);
        state.camera.position.y = state.camera.radius * sin(polRad);
        state.camera.position.z = state.camera.radius * cos(polRad) * cos(azRad);
    }

    bool _HandleIdle(AtelieState& state, int key) {
        switch (key) {
            case GLFW_KEY_Z: state.editor.wireframe = !state.editor.wireframe; break;
            //---
            case GLFW_KEY_TAB: state.editor.editMode = !state.editor.editMode; break;
            //---
            case GLFW_KEY_W: state.camera.polar += 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_S: state.camera.polar -= 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_A: state.camera.azimuth -= 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_D: state.camera.azimuth += 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_EQUAL: state.camera.radius -= 0.25f; break;
            case GLFW_KEY_MINUS: state.camera.radius += 0.25f; break;
            // ---
            case GLFW_KEY_SPACE:
                if (!state.multiselect) {
                    state.multiselect = true;
                    std::fill(state.selected.begin(), state.selected.end(), false);
                }
                state.selected[state.cursor] = !state.selected[state.cursor];
                break;
            // ---
            case GLFW_KEY_RIGHT_BRACKET: state.cursor = (state.cursor + 1) % state.selected.size(); break;
            case GLFW_KEY_LEFT_BRACKET: state.cursor = (state.cursor == 0) ? state.selected.size() - 1 : state.cursor - 1; break;
            // ---
            case GLFW_KEY_BACKSLASH: state.editor.tool = ActiveTool::Increment; break;
            case GLFW_KEY_V: state.editor.tool = ActiveTool::View; break;
            case GLFW_KEY_G: state.editor.tool = ActiveTool::Translate; break;
            default: return false;
        }
        return true;
    }

    bool _HandleIncrement(AtelieState& state, int key) {
        switch (key) {
            case GLFW_KEY_LEFT_BRACKET:
                state.editor.increment -= 0.1f;
                break;
            case GLFW_KEY_RIGHT_BRACKET:
                state.editor.increment += 0.1f;
                break;
            default: return false;
        }
        state.editor.increment = (state.editor.increment > 9.9f) ? 9.9f : state.editor.increment;
        state.editor.increment = (state.editor.increment <= 0.05f) ? 0.1f : state.editor.increment;
        return true;
    }

    bool _HandleView(AtelieState& state, int key) {
        switch (key) {
            case GLFW_KEY_X:
                if (state.camera.orientation == CameraOrientation::SnappedX) {
                    state.camera.polar = 0.0f;
                    state.camera.azimuth = -90.0f;
                    state.camera.orientation = CameraOrientation::Free;
                } else {
                    state.camera.polar = 0.0f;
                    state.camera.azimuth = 90.0f;
                    state.camera.orientation = CameraOrientation::SnappedX;
                }
                break;
            case GLFW_KEY_Y:
                if (state.camera.orientation == CameraOrientation::SnappedY) {
                    state.camera.polar = -90.0f;
                    state.camera.azimuth = 0.0f;
                    state.camera.orientation = CameraOrientation::Free;
                } else {
                    state.camera.polar = 90.0f;
                    state.camera.azimuth = 0.0f;
                    state.camera.orientation = CameraOrientation::SnappedY;
                }
                break;
            case GLFW_KEY_Z:
                if (state.camera.orientation == CameraOrientation::SnappedZ) {
                    state.camera.polar = 0.0f;
                    state.camera.azimuth = 180.0f;
                    state.camera.orientation = CameraOrientation::Free;
                } else {
                    state.camera.polar = 0.0f;
                    state.camera.azimuth = 0.0f;
                    state.camera.orientation = CameraOrientation::SnappedZ;
                }
                break;
            default: return false;
        }
        state.editor.tool = ActiveTool::None;
        return true;
    }

    bool _HandleTranslate(AtelieState& state, int key) {
        switch (key) {
            case GLFW_KEY_W:
                state.editor.values.y += state.editor.increment;
                break;
            case GLFW_KEY_S:
                state.editor.values.y -= state.editor.increment;
                break;
            case GLFW_KEY_A:
                state.editor.values.x -= state.editor.increment;
                break;
            case GLFW_KEY_D:
                state.editor.values.x += state.editor.increment;
                break;
            case GLFW_KEY_X:
                state.editor.constraints.x = !state.editor.constraints.x;
                state.editor.constraints.y = false;
                state.editor.constraints.z = false;
                break;
            case GLFW_KEY_Y:
                state.editor.constraints.x = false;
                state.editor.constraints.y = !state.editor.constraints.y;
                state.editor.constraints.z = false;
                break;
            case GLFW_KEY_Z:
                state.editor.constraints.x = false;
                state.editor.constraints.y = false;
                state.editor.constraints.z = !state.editor.constraints.z;
                break;
            case GLFW_KEY_L:
                state.editor.constraints.local = !state.editor.constraints.local;
                break;
            default: return false;
        }
        state.editor.previewTranslate = glm::vec3(state.editor.values.x, state.editor.values.y, 0.0f);
        return true;
    }

    void _ApplyPreview(AtelieState& state) {
        for (int i = 0; i < state.scene.size(); i++) {
            if (i != state.cursor && !state.selected[i]) continue;
            state.scene[i].position += state.editor.previewTranslate;
            // state.scene[i].rotation = glm::rotate(state.scene[i].rotation, state.editor.previewRotate);
            // state.scene[i].scale = glm::scale(state.scene[i].scale, state.editor.previewScale);
        }
    }

    void _KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

        AtelieState* state = (AtelieState*) glfwGetWindowUserPointer(window);
        auto& editor = state->editor;
        auto& transcript = state->transcript;

        bool toolActive = (editor.tool != ActiveTool::None);
        bool record = true;
        bool consumed = true;

        // ======
        // 1. ESCAPE
        // ======
        if (key == GLFW_KEY_ESCAPE) {
            if (toolActive) {
                if (editor.tool == ActiveTool::Increment) goto commit;
                transcript.pending.clear();
                editor.previewTranslate = glm::vec3(0.0f);
                editor.values = glm::vec2(0.0f);
            } else {
                state->multiselect = false;
            }
            editor.tool = ActiveTool::None;
            return;
        }

        // ======
        // 2. ENTER
        // ======
        if (key == GLFW_KEY_ENTER && toolActive) {
            _ApplyPreview(*state);
            editor.previewTranslate = glm::vec3(0.0f);

            commit:
            transcript.pending.push_back({key, mods});
            transcript.committed.insert(
                transcript.committed.end(),
                transcript.pending.begin(),
                transcript.pending.end()
            );
            transcript.pending.clear();
            editor.tool = ActiveTool::None;
            editor.values = glm::vec2(0.0f);
            return;
        }

        // ======
        // 3. FSM
        // ======
        else {
            switch (editor.tool) {
                case ActiveTool::None:
                    consumed = _HandleIdle(*state, key);
                    break;
                case ActiveTool::Increment:
                    consumed = _HandleIncrement(*state, key);
                    break;
                case ActiveTool::View:
                    consumed = _HandleView(*state, key);
                    break;
                case ActiveTool::Translate:
                    consumed = _HandleTranslate(*state, key);
                    break;
            }
        }

        // ======
        // 4. RECORD
        // ======
        if (consumed) {
            if (toolActive) {
                transcript.pending.push_back({key, mods});
            } else {
                transcript.committed.push_back({key, mods});
            }
        }
    }

    // public
    void Init(GLFWwindow* window) {
        glfwSetKeyCallback(window, _KeyCallback);
    }

    void Process(GLFWwindow* window, AtelieState& state) {
        _UpdateCameraPosition(state);
    }

}