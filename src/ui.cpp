#include "ui.h"

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

    if (ImGui::BeginMainMenuBar()) {
        ImGui::Text(state.title);
        ImGui::Text(state.version);
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