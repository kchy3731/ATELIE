#pragma once

#include <string>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp> 

// TODO: Move this to own header!
struct CameraState {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float fov = 70.0f;
};

struct AtelieState {
    // TODO: INPUT, SCENE, RENDER

    // RENDER
    bool showWireframe = false;

    // UI
    const char* title = "ATELIE";
    const char* version = "v0.0";
    char* mode;
    char* detail;

    CameraState camera;
};