#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "primitives.h"
#include "scene.h"

enum class CameraOrientation { Free, SnappedX, SnappedY, SnappedZ };

enum class ActiveTool { None, Increment, View, Translate, Rotate, Scale, Spawn, Delete };

enum class MeshType { Cube, Triangle, Plane, Cylinder };
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
    float azimuth = 37.5f;
    float polar = 30.0f;

    bool orthographic = false;
    float orthographicSize = 1.25f;

    CameraOrientation orientation = CameraOrientation::Free;

    glm::vec3 position;
};

struct TransformConstraints {
    bool x;
    bool y;
    bool z;
    bool local;
};

struct EditorState {
    TransformConstraints constraints;
    glm::vec3 previewTranslate = glm::vec3(0.0f);
    glm::quat previewRotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 previewScale = glm::vec3(1.0f);
    glm::vec2 values = glm::vec2(0.0f);
    
    float increment = 0.1f;

    bool editMode = false;
    bool dummyInitialised = false;
    Scene::Object dummy;

    bool wireframe = false;
    ActiveTool tool = ActiveTool::None;

    Scene::BasicObjectType spawnType = Scene::BasicObjectType::Cube;
    bool spawnActive = false;
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

glm::mat3 GetRotBasis(const Scene::Object& obj);
glm::vec3 GetCameraRight(const AtelieState& state);
glm::vec3 GetCameraFront(const AtelieState& state);
glm::vec3 GetCameraUp(const AtelieState& state);

void EnterEditMode(AtelieState& state);
void LeaveEditMode(AtelieState& state);
void EditAdvanceCursor(AtelieState& state);