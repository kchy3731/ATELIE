#include "input.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace Input {
    // private
    void _UpdateCameraPosition(AtelieState& state) {
        state.camera.polar = (state.camera.polar > 90.0f) ? 90.0f : state.camera.polar;
        state.camera.polar = (state.camera.polar < -90.0f) ? -90.0f : state.camera.polar;
        state.camera.orthographicSize = (state.camera.orthographicSize < 0.25f) ? 0.25f : state.camera.orthographicSize;

        float azRad = glm::radians(state.camera.azimuth);
        float polRad = glm::radians(state.camera.polar);

        state.camera.position.x = state.camera.radius * cos(polRad) * sin(azRad);
        state.camera.position.y = state.camera.radius * sin(polRad);
        state.camera.position.z = state.camera.radius * cos(polRad) * cos(azRad);
    }

    bool _HandleIdle(AtelieState& state, int key) {
        switch (key) {
            case GLFW_KEY_Z: state.editor.wireframe = !state.editor.wireframe; break;
            case GLFW_KEY_O: state.camera.orthographic = !state.camera.orthographic; break;
            //---
            case GLFW_KEY_TAB: state.editor.editMode = !state.editor.editMode; break;
            //---
            case GLFW_KEY_W: state.camera.polar += 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_S: state.camera.polar -= 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_A: state.camera.azimuth -= 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_D: state.camera.azimuth += 10.0f; state.camera.orientation = CameraOrientation::Free; break;
            case GLFW_KEY_EQUAL: state.camera.radius -= 0.25f; state.camera.orthographicSize -= 0.25f; break;
            case GLFW_KEY_MINUS: state.camera.radius += 0.25f; state.camera.orthographicSize += 0.25f; break;
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
            // ---
            case GLFW_KEY_G: state.editor.tool = ActiveTool::Translate; break;
            case GLFW_KEY_R: state.editor.tool = ActiveTool::Rotate; break;
            case GLFW_KEY_M: state.editor.tool = ActiveTool::Scale; break;
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

    float AxisSign(const glm::vec3& camVec, const glm::vec3& axis, const glm::mat3& localBasisInv, bool local) {
        glm::vec3 v = local ? (localBasisInv * camVec) : camVec;
        return (glm::dot(v, axis) > 0) ? 1.0f : -1.0f;
    }

    bool SteeperThan45(glm::vec3 v) {
        v = glm::normalize(v);
        return std::abs(v.y) > 0.7071f;  // √2 / 2
    }

    bool _HandleTranslate(AtelieState& state, int key) {
        float& increment = state.editor.increment;
        glm::vec2& values = state.editor.values;
        TransformConstraints& constraints = state.editor.constraints;

        switch (key) {
            case GLFW_KEY_W:
                values.y += increment;
                break;
            case GLFW_KEY_S:
                values.y -= increment;
                break;
            case GLFW_KEY_A:
                values.x -= increment;
                break;
            case GLFW_KEY_D:
                values.x += increment;
                break;
            case GLFW_KEY_X:
                constraints.x = !constraints.x;
                constraints.y = false;
                constraints.z = false;
                break;
            case GLFW_KEY_Y:
                constraints.x = false;
                constraints.y = !constraints.y;
                constraints.z = false;
                break;
            case GLFW_KEY_Z:
                constraints.x = false;
                constraints.y = false;
                constraints.z = !constraints.z;
                break;
            case GLFW_KEY_L:
                constraints.local = !constraints.local;
                break;
            default: return false;
        }

        glm::vec3 translate = glm::vec3(0.0f);
        glm::vec3 camRight = GetCameraRight(state);
        glm::vec3 camFront = GetCameraFront(state);
        glm::mat3 anchorRotBasis = GetRotBasis(state.scene[state.cursor]);
        if (!constraints.x && !constraints.y && !constraints.z && !constraints.local) {
            glm::vec3 camUp = GetCameraUp(state);
            translate = camRight * values.x + camUp * values.y;
        }
        if (constraints.x) {
            float sign = AxisSign(camRight, glm::vec3(1.0f, 0.0f, 0.0f), glm::transpose(anchorRotBasis), constraints.local);
            translate.x += values.x * sign;
        }
        if (constraints.y) {
            translate.y += values.y;
        }
        if (constraints.z) {
            float sign = AxisSign(camRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::transpose(anchorRotBasis), constraints.local);
            translate.z += values.x * sign;
        }
        if (constraints.local) translate = anchorRotBasis * translate;
        state.editor.previewTranslate = translate;
        return true;
    }

    bool _HandleRotate(AtelieState& state, int key) {
        float& increment = state.editor.increment;
        glm::vec2& values = state.editor.values;
        TransformConstraints& constraints = state.editor.constraints;

        switch (key) {
            case GLFW_KEY_W:
                values.y += increment;
                break;
            case GLFW_KEY_S:
                values.y -= increment;
                break;
            case GLFW_KEY_A:
                values.x -= increment;
                break;
            case GLFW_KEY_D:
                values.x += increment;
                break;
            case GLFW_KEY_X:
                constraints.x = !constraints.x;
                constraints.y = false;
                constraints.z = false;
                break;
            case GLFW_KEY_Y:
                constraints.x = false;
                constraints.y = !constraints.y;
                constraints.z = false;
                break;
            case GLFW_KEY_Z:
                constraints.x = false;
                constraints.y = false;
                constraints.z = !constraints.z;
                break;
            case GLFW_KEY_L:
                constraints.local = !constraints.local;
                break;
            default: return false;
        }

        glm::quat rotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 camRight = GetCameraRight(state);
        glm::vec3 camFront = GetCameraFront(state);
        glm::mat3 anchorRotBasis = GetRotBasis(state.scene[state.cursor]);
        float valueX = values.x * 50; // 0.1 * 10 * 5 = 5deg
        float valueY = values.y * 50; // 0.1 * 10 * 5 = 5deg
        if (!constraints.x && !constraints.y && !constraints.z && !constraints.local) {
            rotate = glm::angleAxis(glm::radians(valueX), camFront);
        }
        if (constraints.x) {
            glm::vec3 axis = glm::vec3(1.0f, 0.0f, 0.0f);
            float sign = (glm::dot(axis, camFront) < 0) ? -1.0f : 1.0f;
            rotate = glm::angleAxis(glm::radians(valueY * sign), axis);
        }
        if (constraints.y) {
            glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);
            rotate = glm::angleAxis(glm::radians(valueX), axis);
        }
        if (constraints.z) {
            glm::vec3 axis = glm::vec3(0.0f, 0.0f, 1.0f);
            float sign = (glm::dot(axis, camFront) > 0) ? -1.0f : 1.0f;
            rotate = glm::angleAxis(glm::radians(valueY * sign), axis);
        }
        if (constraints.local) {
            glm::vec3 axis;
            if (constraints.x) axis = anchorRotBasis[0];
            else if (constraints.y) axis = anchorRotBasis[1];
            else if (constraints.z) axis = anchorRotBasis[2];

            float sign = (glm::dot(axis, camFront) > 0) ? -1.0f : 1.0f;
            float value = (SteeperThan45(axis)) ? valueY : valueX;
            float angle = value * sign;
            rotate = glm::angleAxis(glm::radians(angle), axis);
        }
        state.editor.previewRotate = rotate;
        return true;
    }

    bool _HandleScale(AtelieState& state, int key) {
        float& increment = state.editor.increment;
        glm::vec2& values = state.editor.values;
        TransformConstraints& constraints = state.editor.constraints;

        switch (key) {
            case GLFW_KEY_W:
                values.y += increment;
                break;
            case GLFW_KEY_S:
                values.y -= increment;
                break;
            case GLFW_KEY_A:
                values.x -= increment;
                break;
            case GLFW_KEY_D:
                values.x += increment;
                break;
            case GLFW_KEY_X:
                constraints.x = !constraints.x;
                constraints.y = false;
                constraints.z = false;
                break;
            case GLFW_KEY_Y:
                constraints.x = false;
                constraints.y = !constraints.y;
                constraints.z = false;
                break;
            case GLFW_KEY_Z:
                constraints.x = false;
                constraints.y = false;
                constraints.z = !constraints.z;
                break;
            case GLFW_KEY_L:
                constraints.local = !constraints.local;
                break;
            default: return false;
        }

        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 camRight = GetCameraRight(state);
        glm::vec3 camFront = GetCameraFront(state);
        glm::mat3 anchorRotBasis = GetRotBasis(state.scene[state.cursor]);
        if (!constraints.x && !constraints.y && !constraints.z && !constraints.local) {
            scale.x += values.y;
            scale.y += values.y;
            scale.z += values.y;
        }
        if (constraints.x) {
            scale.x += values.y;
        }
        if (constraints.y) {
            scale.y += values.y;
        }
        if (constraints.z) {
            scale.z += values.y;
        }
        if (constraints.local) {
            glm::vec3 axis = glm::vec3(1.0f, 1.0f, 1.0f);
            if (constraints.x) axis = anchorRotBasis[0];
            else if (constraints.y) axis = anchorRotBasis[1];
            else if (constraints.z) axis = anchorRotBasis[2];

            scale += (glm::normalize(axis) * values.y);
        }
        state.editor.previewScale = scale;
        return true;
    }

    void _CancelPreview(AtelieState& state) {
        state.editor.constraints.x = false;
        state.editor.constraints.y = false;
        state.editor.constraints.z = false;
        state.editor.constraints.local = false;
        state.editor.previewTranslate = glm::vec3(0.0f);
        state.editor.previewRotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        state.editor.previewScale = glm::vec3(1.0f, 1.0f, 1.0f);
        state.editor.values = glm::vec2(0.0f);
    }

    void _ApplyPreview(AtelieState& state) {
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
        _CancelPreview(state);
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
            } else {
                state->multiselect = false;
                std::fill(state->selected.begin(), state->selected.end(), false);
            }
            _CancelPreview(*state);
            editor.tool = ActiveTool::None;
            return;
        }

        // ======
        // 2. ENTER
        // ======
        if (key == GLFW_KEY_ENTER && toolActive) {
            _ApplyPreview(*state);

            commit:
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
                case ActiveTool::Increment:
                    consumed = _HandleIncrement(*state, key);
                    break;
                case ActiveTool::View:
                    consumed = _HandleView(*state, key);
                    break;
                case ActiveTool::Translate:
                    consumed = _HandleTranslate(*state, key);
                    break;
                case ActiveTool::Rotate:
                    consumed = _HandleRotate(*state, key);
                    break;
                case ActiveTool::Scale:
                    consumed = _HandleScale(*state, key);
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