#include "raylib.h"
#include "raymath.h"

#include "imgui.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"

#include "command.h"
#include "editor.h"
#include "map_editor.h"
#include "editor_commands.h"

#include "views/editor_view.h"
#include "panels/edit_history.h"
#include "panels/material_picker_panel.h"
#include "panels/toolbar_panel.h"
#include "utils/imgui_dialogs.h"

namespace Editor
{
    bool WantQuit = false;

    MapEditor ActiveEditor;
    MapEditor& GetActiveEditor() { return ActiveEditor; }

    EditorView ActiveView(ActiveEditor);
    EditorView& GetActiveView() { return ActiveView; }

    std::vector<Panel*> Panels;

    void Quit()
    {
        WantQuit = true;
    }

    void Shutdown()
    {
        ActiveView.Shutdown();

        for (auto* panel : Panels)
            delete(panel);

        Panels.clear();
    }

    void Update()
    {
        EditorCommands::GetCommandSet().CheckShortcuts();

        ActiveEditor.Update();

        ActiveView.HasFocus = !ImGui::GetIO().WantCaptureMouse;
		if (!ActiveView.HasFocus)
			return;

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            ActiveView.Pan(GetMouseDelta());
        ActiveView.Zoom(GetMouseWheelMove());
    }

    void Startup()
    {
        Panels.emplace_back(new EditHistoryPanel()); 
        Panels.emplace_back(new MaterialPickerPanel()); 
        Panels.emplace_back(new ToolbarPanel());
    }

    void MainMenu()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                EditorCommands::NewMap.Menu();
                EditorCommands::OpenMap.Menu();

                ImGui::Separator();
                EditorCommands::SaveMap.Menu();
                EditorCommands::SaveMapAs.Menu();

                ImGui::Separator();
                EditorCommands::Quit.Menu();

                ImGui::EndMenu();
            }
			if (ImGui::BeginMenu("Edit"))
			{
				EditorCommands::Undo.Menu();
				EditorCommands::Redo.Menu();

                ImGui::Separator();
                EditorCommands::Resize.Menu();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
                for (auto* panel : Panels)
                {
                    if (ImGui::MenuItem(panel->GetName(), nullptr, panel->IsVisible()))
                        panel->SetVisible(!panel->IsVisible());
                }
				ImGui::EndMenu();
			}

            ImGui::EndMainMenuBar();
        }
    }

    void ShowUI()
    {
        MainMenu();

        for (auto* panel : Panels)
            panel->Show();

        ImGui::UpdateDialogs();
    }

    void ShowContent()
    {
        ActiveView.Show();
    }
}

int main(int argc, char* argv[])
{
	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 1900;
	int screenHeight = 900;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "raycasterPro Editor");
    SetExitKey(KEY_NULL);
	SetTargetFPS(144);

    Image icon = LoadImage("resources/editor_icon.png");
    ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(icon);
    UnloadImage(icon);

	rlImGuiSetup(true);
	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    Editor::Startup();

	// Main game loop
    while (!WindowShouldClose() && !Editor::WantQuit)    // Detect window close button or ESC key
	{
        Editor::Update();

		BeginDrawing();
		ClearBackground(DARKGRAY);

        Editor::ShowContent();

		rlImGuiBegin();
        Editor::ShowUI();
		rlImGuiEnd();

		EndDrawing();
		//----------------------------------------------------------------------------------
	}
	rlImGuiShutdown();

    Editor::Shutdown();
	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}