#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "state.h"

#include "input.h"
#include "render.h"
#include "ui.h"

int main() {

    // WINDOW SETUP
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "ATELIE", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    AtelieState state;
    glfwSetWindowUserPointer(window, &state);

    Input::Init(window);
    Render::Init();
    UI::Init(window);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Input::Process(window, state);
        Render::Draw(state);
        UI::Process(state);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Render::Cleanup();
    UI::Cleanup();

    glfwTerminate();
    return 0;
}
