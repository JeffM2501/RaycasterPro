#include "raylib.h"
#include "raymath.h"

#include "imgui.h"
#include "rlImGui.h"
#include "rlImGuiColors.h"

#include "command.h"
#include "editor.h"
#include "editor_commands.h"

namespace Editor
{
    bool WantQuit = false;

    MapEditor ActiveEditor;
    MapEditor& GetActiveEditor() { return ActiveEditor; }

    void Quit()
    {
        WantQuit = true;
    }

    void Shutdown()
    {

    }

    void Update()
    {
        EditorCommands::Commands.CheckShortcuts();
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
            ImGui::EndMainMenuBar();
        }
    }

    void ShowUI()
    {
        MainMenu();
    }

    void ShowContent()
    {

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

        ImGui::ShowDemoWindow(nullptr);
       
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