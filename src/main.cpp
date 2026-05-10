#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --- Shaders ---
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main() { gl_Position = projection * view * model * vec4(aPos, 1.0); }\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() { FragColor = vec4(0.8f, 0.4f, 0.1f, 1.0f); }\n\0";
struct Mesh {
    unsigned int VAO;
    unsigned int VBO;
    int vertexCount;
};

Mesh createMesh(const float* vertices, int size, int count) {
    Mesh mesh;
    mesh.vertexCount = count;

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    // Tell OpenGL how to read the positions (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return mesh;
}

// --- Camera State ---
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0f / 2.0;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --- Global State for Input ---
// (In a real engine, this goes in your Stage/State Machine class)
int currentMeshIndex = 0; 
bool key1_pressed_last_frame = false;
bool key2_pressed_last_frame = false;


#include "state.h"
#include "primitives.h"
#include "scene.h"

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
    if (window == NULL) { std::cout << "Failed to create GLFW window\n"; glfwTerminate(); return -1; }
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

    state.scene.push_back(Scene::CreateSceneObject(Scene::BasicObjectType::Cube));
    state.selected.push_back(false);
    state.scene.push_back(Scene::CreateSceneObject(Scene::BasicObjectType::Cube));
    state.selected.push_back(false);
    state.scene.push_back(Scene::CreateSceneObject(Scene::BasicObjectType::Cube));
    state.selected.push_back(false);
    state.scene.push_back(Scene::CreateSceneObject(Scene::BasicObjectType::Cube));
    state.selected.push_back(false);
    state.scene.push_back(Scene::CreateSceneObject(Scene::BasicObjectType::Cube));
    state.selected.push_back(false);
    state.scene[0].position = glm::vec3(0.0f, -1.0f, 0.0f);
    state.scene[1].position = glm::vec3(1.0f, 0.0f, 0.0f);
    state.scene[2].position = glm::vec3(-2.0f, 0.0f, 0.0f);
    state.scene[3].position = glm::vec3(0.5f, 2.0f, 1.0f);
    state.scene[4].position = glm::vec3(-0.7f, 1.5f, 0.0f);

    state.cursor = 0;
    state.editor.constraints.x = false;
    state.editor.constraints.y = false;
    state.editor.constraints.z = false;
    state.editor.constraints.local = false;

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
