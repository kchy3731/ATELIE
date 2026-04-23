#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
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

std::string inputBuffer = "";
glm::mat4 customTransform = glm::mat4(1.0f);

void character_callback(GLFWwindow* window, unsigned int codepoint) {
    if (codepoint >= '0' && codepoint <= '9') {
        inputBuffer += (char)codepoint;
        std::cout << "Buffer: " << inputBuffer << std::endl;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        firstMouse = true;
        return;
    }

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;

    bool enter_pressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
    static bool enter_pressed_last_frame = false;
    if (enter_pressed && !enter_pressed_last_frame) {
        if (!inputBuffer.empty()) {
            std::cout << "Executing instruction: " << inputBuffer << std::endl;
            char cat = inputBuffer[0];
            
            if (cat == '0') {
                customTransform = glm::mat4(1.0f);
            } else if (cat == '2') {
                if (inputBuffer.length() > 1) {
                    int meshIdx = inputBuffer[1] - '0';
                    if (meshIdx == 0 || meshIdx == 1) currentMeshIndex = meshIdx;
                }
            } else if (cat == '1') {
                if (inputBuffer.length() >= 4) {
                    char op = inputBuffer[1];
                    char axis = inputBuffer[2];
                    char sign = inputBuffer[3];
                    float val = 1.0f;
                    
                    if (inputBuffer.length() > 4) {
                        try {
                            val = std::stof(inputBuffer.substr(4));
                        } catch(...) { val = 1.0f; }
                    }

                    if (op == '1') { // Scale
                        if (sign == '1') val = 1.0f / (val != 0.0f ? val : 1.0f);
                    } else { // Translate or Rotate
                        if (sign == '1') val = -val;
                    }

                    glm::vec3 axisVec(0.0f);
                    if (axis == '1') axisVec.x = 1.0f;
                    else if (axis == '2') axisVec.y = 1.0f;
                    else if (axis == '3') axisVec.z = 1.0f;
                    else if (axis == '4') axisVec = glm::vec3(1.0f);

                    if (op == '1') {
                        glm::vec3 scaleVec = (axis == '4') ? glm::vec3(val) : (axisVec * (val - 1.0f) + glm::vec3(1.0f));
                        customTransform = glm::scale(customTransform, scaleVec);
                    } else if (op == '2') {
                        customTransform = glm::translate(customTransform, axisVec * val);
                    } else if (op == '3') {
                        if (axis != '4') {
                            customTransform = glm::rotate(customTransform, glm::radians(val), axisVec);
                        }
                    }
                } else {
                    std::cout << "Incomplete transform instruction." << std::endl;
                }
            } else {
                std::cout << "Unknown command category." << std::endl;
            }
            inputBuffer.clear();
        }
    }
    enter_pressed_last_frame = enter_pressed;

    bool backspace_pressed = glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS;
    static bool backspace_pressed_last_frame = false;
    if (backspace_pressed && !backspace_pressed_last_frame) {
        if (!inputBuffer.empty()) {
            inputBuffer.pop_back();
            std::cout << "Buffer: " << inputBuffer << std::endl;
        }
    }
    backspace_pressed_last_frame = backspace_pressed;

}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "ATELIE", NULL, NULL);
    if (window == NULL) { std::cout << "Failed to create GLFW window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetCharCallback(window, character_callback);

    // tell GLFW to capture our mouse (normal mode so we can click safely)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "Failed to initialize GLAD\n"; return -1; }
    glEnable(GL_DEPTH_TEST);

    // --- Compile Shaders ---
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // --- Create Geometry ---
    // Shape 0: A simple 3D Pyramid
    float triVertices[] = {
        // Base (2 triangles)
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        // Front face
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.0f,  0.5f,  0.0f,

        // Right face
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.0f,  0.5f,  0.0f,

        // Back face
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
         0.0f,  0.5f,  0.0f,

        // Left face
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.0f,  0.5f,  0.0f
    };
    
    // Shape 1: A simple 3D Cube (36 vertices)
    float quadVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };

    Mesh triMesh = createMesh(triVertices, sizeof(triVertices), 18);
    Mesh quadMesh = createMesh(quadVertices, sizeof(quadVertices), 36);
    
    // Store them in an array so we can index them with our keyboard logic
    Mesh sceneMeshes[2] = {triMesh, quadMesh};

    

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the currently selected mesh
        glUseProgram(shaderProgram);

        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --- Create 3D Camera Math ---
        glm::mat4 model = customTransform;

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Perspective projection: FOV, aspect ratio, near, far clip planes
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Pass matrices to shader
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(sceneMeshes[currentMeshIndex].VAO);
        glDrawArrays(GL_TRIANGLES, 0, sceneMeshes[currentMeshIndex].vertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}