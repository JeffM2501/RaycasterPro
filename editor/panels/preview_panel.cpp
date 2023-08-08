#include "panels/preview_panel.h"

#include "extras/IconsFontAwesome6.h"
#include "editor.h"
#include "rlImGui.h"

PreviewPanel::PreviewPanel()
    : Caster(nullptr, 600, 65)
    , Renderer(Caster, nullptr)
{
    Name = "Preview";
    HorizontalAlignment = Panel::Alignment::Maxium;
    VerticalAlignment = Panel::Alignment::Minium;

    Size = ImVec2(600, 440);
    Offset.x = 200;
}

void PreviewPanel::OnShow()
{
    if (PreviewTexture.id == 0)
    {
        Renderer.SetTileTexture(Editor::GetActiveView().GetTileTexture());
        PreviewTexture = LoadRenderTexture(600,400);
    }

    const Map& map = Editor::GetActiveEditor().GetWorkingMap();
    Caster.SetMap(&map);
    Renderer.SetMap(&map);

    EntityLocation& loc = Editor::GetActiveEditor().GetViewLocation();

    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        float rot = GetMouseDelta().x * 0.75f;
        loc.Facing = Vector2Rotate(loc.Facing, -rot * DEG2RAD);

        Vector2 sideStepVector = { -loc.Facing.y, loc.Facing.x };

        float speed = 5 * GetFrameTime();
        if (IsKeyDown(KEY_W))
            loc.Position = Vector2Add(loc.Position, Vector2Scale(loc.Facing, speed));
		if (IsKeyDown(KEY_S))
			loc.Position = Vector2Add(loc.Position, Vector2Scale(loc.Facing, -speed));
		if (IsKeyDown(KEY_A))
			loc.Position = Vector2Add(loc.Position, Vector2Scale(sideStepVector, speed));
		if (IsKeyDown(KEY_D))
			loc.Position = Vector2Add(loc.Position, Vector2Scale(sideStepVector, -speed));
    }
    Caster.StartFrame(loc);

    BeginTextureMode(PreviewTexture);
    ClearBackground(BLACK);
    Renderer.Draw(loc);
    EndTextureMode();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
    rlImGuiImageRect(&PreviewTexture.texture, PreviewTexture.texture.width, PreviewTexture.texture.height, Rectangle{ 0,0, float(PreviewTexture.texture.width),float(-PreviewTexture.texture.height) });
    ImGui::PopStyleVar();

}
