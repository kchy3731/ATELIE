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
            case GLFW_KEY_V: state.editor.tool = ActiveTool::View; break;
            // case GLFW_KEY_G: state.editor.tool = ActiveTool::Translate; break;
            default: return false;
        }
        return true;
    }

    bool _HandleView(AtelieState& state, int key) {
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
        bool record = true;
        bool consumed = true;

        // ======
        // 1. ESCAPE
        // ======
        if (key == GLFW_KEY_ESCAPE) {
            if (toolActive) {
                transcript.pending.clear();
                editor.previewTranslate = glm::vec3(0.0f);
            }
            state->multiselect = false;
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

    // void _MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    //     if (action != GLFW_PRESS) return;

    //     // Don't click through the UI
    //     if (ImGui::GetIO().WantCaptureMouse) return;

    //     if (button == GLFW_MOUSE_BUTTON_LEFT) {
    //         AtelieState* state = (AtelieState*) glfwGetWindowUserPointer(window);
    //         double xpos, ypos;
    //         glfwGetCursorPos(window, &xpos, &ypos);

    //         int pickedID = Render::PickObject(*state, xpos, ypos);

    //         // Handle selection
    //         if (!(mods & GLFW_MOD_CONTROL)) {
    //             state->editor.selection.clear();
    //         }

    //         if (pickedID >= 0 && pickedID < state->scene.size()) {
    //             auto it = std::find(state->editor.selection.begin(), state->editor.selection.end(), pickedID);
    //             if (it != state->editor.selection.end()) {
    //                 // Already selected, toggle off if Ctrl is held
    //                 if (mods & GLFW_MOD_CONTROL) {
    //                     state->editor.selection.erase(it);
    //                 }
    //             } else {
    //                 state->editor.selection.push_back(pickedID);
    //             }
    //         }
    //     }
    // }

    // public
    void Init(GLFWwindow* window) {
        glfwSetKeyCallback(window, _KeyCallback);
        // glfwSetMouseButtonCallback(window, _MouseButtonCallback);
    }

    void Process(GLFWwindow* window, AtelieState& state) {
        _UpdateCameraPosition(state);
    }

}