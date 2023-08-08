#include "panels/preview_panel.h"

#include "extras/IconsFontAwesome6.h"
#include "editor.h"
#include "rlImGui.h"

PreviewPanel::PreviewPanel()
    : Caster(nullptr, 400, 45)
    , Renderer(Caster, nullptr)
{
    Name = "Preview";
    HorizontalAlignment = Panel::Alignment::Maxium;
    VerticalAlignment = Panel::Alignment::Maxium;

    Size = ImVec2(400, 440);
    Offset.x = 200;
}

void PreviewPanel::OnShow()
{
    if (PreviewTexture.id == 0)
    {
        Renderer.SetTileTexture(Editor::GetActiveView().GetTileTexture());
        PreviewTexture = LoadRenderTexture(400,400);
    }

    const Map& map = Editor::GetActiveEditor().GetWorkingMap();
    Caster.SetMap(&map);
    Renderer.SetMap(&map);

    EntityLocation loc;
    loc.Position.x = 5;
    loc.Position.y = 5;
    loc.Facing.x = 1;
    loc.Facing.y = 0;

    Caster.StartFrame(loc);

    BeginTextureMode(PreviewTexture);
    ClearBackground(BLACK);
    Renderer.Draw(loc);
    EndTextureMode();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
    rlImGuiImageRect(&PreviewTexture.texture, PreviewTexture.texture.width, PreviewTexture.texture.height, Rectangle{ 0,0, float(PreviewTexture.texture.width),float(PreviewTexture.texture.height) });
    ImGui::PopStyleVar();

}
