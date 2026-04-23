#include "input.h"

namespace Input {
    void Process(GLFWwindow* window, AtelieState& state) {
        float speed = 0.005f;
        
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            state.camera.position.x += speed;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            state.camera.position.x -= speed;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            state.camera.position.y += speed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            state.camera.position.y -= speed;
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
            state.camera.fov -= 0.05f;
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
            state.camera.fov += 0.05f;

        if (state.camera.fov > 90.0f) state.camera.fov = 90.0f;
        if (state.camera.fov < 30.0f) state.camera.fov = 30.0f;
    }
}