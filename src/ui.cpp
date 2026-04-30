#include "ui.h"

// "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
char* statusString;

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

    statusString = (char*) malloc(strlen("                                                         X\0"));
    strcpy(statusString, "                                                         S");
}

void _ProcessStatusString(AtelieState& state) {
    if (state.editor.wireframe) statusString[57] = 'W';
    else statusString[57] = 'S';
    if (state.editor.editMode) strncpy(statusString + 46, "EDT", 3);
    else strncpy(statusString + 46, "OBJ", 3);
    switch (state.editor.tool) {
        case (ActiveTool::None):
            strncpy(statusString + 20, "    ", 4);
            break;
        case (ActiveTool::View):
            strncpy(statusString + 20, "VIEW", 4);
            break;
    }
}

void UI::Process(AtelieState& state) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("DEBUG");
    ImGui::Text("ATELIE v0.0");
    ImGui::Checkbox("Wireframe Mode", &state.editor.wireframe);
    ImGui::End();

    _ProcessStatusString(state);
    if (ImGui::BeginMainMenuBar()) {
        ImGui::Text(state.ui.title);
        ImGui::Text(statusString);
        ImGui::EndMainMenuBar();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    free(statusString);
}