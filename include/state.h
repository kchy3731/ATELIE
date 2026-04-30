#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include <glm.hpp>

#include "primitives.h"
#include "scene.h"

enum class ProjectionType { Perspective, Orthographic };
enum class CameraOrientation { Free, SnappedX, SnappedY, SnappedZ };

enum class ActiveTool { None, View, Translate /* Fill out! */};

enum class MeshType { Cube, Triangle, Plane };
enum class ObjectType { Static, Dynamic, Light };

struct Keystroke {
    int key;
    int mods;
};

struct Transcript {
    std::vector<Keystroke> committed;
    std::vector<Keystroke> pending;
};

struct UIState {
    const char* title = "ATELIE";
    const char* version = "v0.0";
    char* mode = nullptr;
    char* detail = nullptr;
};

// TODO: Move this to own header! Maybe!
struct CameraState {
    float radius = 5.0f;
    float azimuth = 50.0f;
    float polar = 30.0f;

    ProjectionType projection = ProjectionType::Perspective;
    CameraOrientation orientation = CameraOrientation::Free;

    glm::vec3 position;
};

struct EditorState {
    glm::vec3 previewTranslate = glm::vec3(0.0f);

    bool editMode = false;
    bool wireframe = false;
    ActiveTool tool = ActiveTool::None;
};

// Our global struct! As Atlas holds up the Earth...
struct AtelieState {
    // SAVE/LOAD
    Transcript transcript;

    // SCENE
    unsigned int cursor = 0;
    bool multiselect = false;
    std::vector<bool> selected;
    std::vector<Scene::Object> scene;

    UIState ui;
    CameraState camera;
    EditorState editor;
};