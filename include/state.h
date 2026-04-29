#pragma once

#include <string>
#include <vector>

#include <glm.hpp>

enum class ProjectionType { Perspective, Orthographic };
enum class CameraOrientation { Free, SnappedX, SnappedY, SnappedZ };

enum class ActiveTool { None, View, Translate /* Fill out! */};

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
    std::vector<int> selection;

    ActiveTool tool = ActiveTool::None;
};

// Our global struct! As Atlas holds up the Earth...
struct AtelieState {
    // SAVE/LOAD
    Transcript transcript;

    // RENDER
    bool showWireframe = false;

    UIState ui;
    CameraState camera;
    EditorState editor;
};