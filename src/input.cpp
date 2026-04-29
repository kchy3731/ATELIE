#include "input.h"

namespace Input {
    // private
    void _UpdateCameraPosition(AtelieState& state) {
        float azRad = glm::radians(state.camera.azimuth);
        float polRad = glm::radians(state.camera.polar);

        state.camera.position.x = state.camera.radius * cos(polRad) * sin(azRad);
        state.camera.position.y = state.camera.radius * sin(polRad);
        state.camera.position.z = state.camera.radius * cos(polRad) * cos(azRad);
    }

    bool _HandleIdle(AtelieState& state, int key) {
        switch (key) {
            case GLFW_KEY_W: state.camera.polar -= 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_S: state.camera.polar += 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_A: state.camera.azimuth += 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_D: state.camera.azimuth -= 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            // ---
            case GLFW_KEY_V: state.editor.tool = ActiveTool::View; break;
            // case GLFW_KEY_T: state.editor.tool = ActiveTool::Translate; break;
            default: return false;
        }
        return true;
    }

    bool _HandleView(AtelieState& state, int key) {
        state.editor.tool = ActiveTool::None;
        switch (key) {
            case GLFW_KEY_X:
                if (state.camera.orientation == CameraOrientation::SnappedX) {
                    state.camera.polar = 0.0f;
                    state.camera.azimuth = 90.0f;
                    state.camera.orientation = CameraOrientation::Free;
                } else {
                    state.camera.polar = 0.0f;
                    state.camera.azimuth = -90.0f;
                    state.camera.orientation = CameraOrientation::SnappedX;
                }
                break;
            default: return false;
        }
        return true;
    }

    void _KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

        AtelieState* state = (AtelieState*) glfwGetWindowUserPointer(window);
        auto& editor = state->editor;
        auto& transcript = state->transcript;

        bool toolActive = (editor.tool != ActiveTool::None);
        bool consumed = true;

        // ======
        // 1. ESCAPE
        // ======
        if (key == GLFW_KEY_ESCAPE) {
            if (toolActive) {
                transcript.pending.clear();
                editor.previewTranslate = glm::vec3(0.0f);
                editor.tool = ActiveTool::None;
            }
            editor.tool = ActiveTool::None;
            return;
        }

        // ======
        // 2. ENTER
        // ======
        if (key == GLFW_KEY_ENTER && toolActive) {
            // _ApplyPreview(*state);
            editor.previewTranslate = glm::vec3(0.0f);

            transcript.pending.push_back({key, mods});
            transcript.committed.insert(
                transcript.committed.end(),
                transcript.pending.begin(),
                transcript.pending.end()
            );
            transcript.pending.clear();
            editor.tool = ActiveTool::None;
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
                case ActiveTool::View:
                    consumed = _HandleView(*state, key);
                    break;
                case ActiveTool::Translate:
                    // consumed = _HandleTranslate(*state, key);
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