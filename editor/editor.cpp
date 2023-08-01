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

namespace Editor
{
    bool WantQuit = false;

    MapEditor ActiveEditor;
    MapEditor& GetActiveEditor() { return ActiveEditor; }

    EditorView ActiveView(ActiveEditor);
    EditorView& GetActiveView() { return ActiveView; }

    void Quit()
    {
        WantQuit = true;
    }

    void Shutdown()
    {
        ActiveView.Shutdown();
    }

    void Update()
    {
        EditorCommand::Commands.CheckShortcuts();

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            ActiveView.Pan(GetMouseDelta());
        ActiveView.Zoom(GetMouseWheelMove());
    }

    void Startup()
    {
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

				ImGui::EndMenu();
			}

            ImGui::EndMainMenuBar();
        }
    }

    void ShowUI()
    {
        MainMenu();

        if (ImGui::Begin("Edit History"))
        {
            for (size_t i = 0; i < ActiveEditor.GetEditHistory().size(); i++)
            {
                const auto& item = ActiveEditor.GetEditHistory()[i];
                bool selected = i == ActiveEditor.GetCurrentEditHistoryIndex()-1;
                if (ImGui::Selectable(item.EventName.c_str(), &selected))
                {

                }
            }
        }
        ImGui::End();
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

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, "raycasterPro Editor");
    SetExitKey(KEY_NULL);
	SetTargetFPS(144);

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