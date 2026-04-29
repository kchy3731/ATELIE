#include "ui.h"
#include <algorithm>
#include <string>

void UI::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGuiStyle& style = ImGui::GetStyle();

    // window
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);

    // font
    style.FontSizeBase = 32.0f;
    io.Fonts->AddFontFromFileTTF("./fonts/MBASliceMono-Regular.otf");

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void UI::Process(AtelieState& state) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("DEBUG");
    ImGui::Text("ATELIE v0.0");
    ImGui::Checkbox("Wireframe Mode", &state.showWireframe);
    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0.0f, 600.0f - 300.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Scene Tracker");

    if (ImGui::Button("Add Cube")) {
        SceneObject newCube;
        newCube.name = "Cube " + std::to_string(state.scene.size());
        newCube.meshType = MeshType::Cube;
        newCube.objectType = ObjectType::Static;
        state.scene.push_back(newCube);
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Triangle")) {
        SceneObject newTri;
        newTri.name = "Triangle " + std::to_string(state.scene.size());
        newTri.meshType = MeshType::Triangle;
        newTri.objectType = ObjectType::Static;
        state.scene.push_back(newTri);
    }
    ImGui::Separator();
    for (int i = 0; i < state.scene.size(); i++) {
        auto& obj = state.scene[i];
        
        bool is_selected = false;
        for (int sel : state.editor.selection) {
            if (sel == i) {
                is_selected = true;
                break;
            }
        }

        std::string label = obj.name + "##" + std::to_string(i);
        if (ImGui::Selectable(label.c_str(), is_selected)) {
            // If control isn't held, clear selection
            if (!ImGui::GetIO().KeyCtrl) {
                state.editor.selection.clear();
            }
            // Toggle selection
            if (is_selected) {
                state.editor.selection.erase(std::remove(state.editor.selection.begin(), state.editor.selection.end(), i), state.editor.selection.end());
            } else {
                state.editor.selection.push_back(i);
            }
        }
    }

    if (!state.editor.selection.empty()) {
        ImGui::Separator();
        ImGui::Text("Transform Properties");
        // Get the first selected item to display
        int selected_idx = state.editor.selection.front();
        if (selected_idx >= 0 && selected_idx < state.scene.size()) {
            auto& selected_obj = state.scene[selected_idx];
            ImGui::DragFloat3("Position", &selected_obj.position[0], 0.1f);
            ImGui::DragFloat3("Rotation", &selected_obj.rotation[0], 1.0f);
            ImGui::DragFloat3("Scale", &selected_obj.scale[0], 0.1f);
        }
    }
    ImGui::End();

    if (ImGui::BeginMainMenuBar()) {
        ImGui::Text(state.ui.title);
        ImGui::Text(state.ui.version);
        if (!state.showWireframe) { // TODO
            ImGui::Text("                                                    X");
        } else {
            ImGui::Text("               wireframe                            X");
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}