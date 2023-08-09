#include "tools/editor_tools.h"
#include "editor.h"

SelectTool::SelectTool()
{
    Icon = ICON_FA_ARROW_POINTER;
    ToolTip = "Select";
}

EraserTool::EraserTool()
{
    Icon = ICON_FA_ERASER;
    ToolTip = "Erase";
}

void EraserTool::OnClick(const ImVec2& mapCoordinate)
{
    Editor::GetActiveEditor().SetCell(Vector2i{ int(floorf(mapCoordinate.x)),int(floorf(mapCoordinate.y)) }, 0, EraseAction);
}

PaintWallTool::PaintWallTool()
{
    Icon = ICON_FA_PAINT_ROLLER;
    ToolTip = "Paint Walls";
}

void PaintWallTool::OnClick(const ImVec2& mapCoordinate)
{
    Editor::GetActiveEditor().SetCell(Vector2i{ int(floorf(mapCoordinate.x)),int(floorf(mapCoordinate.y)) }, Editor::GetActiveEditor().GetCurrentMaterial(), PaintWallAction);
}

PaintFloorTool::PaintFloorTool()
{
    Icon = ICON_FA_ARROW_DOWN_LONG;
    ToolTip = "Paint Floors";
}

PaintCeilingTool::PaintCeilingTool()
{
    Icon = ICON_FA_ARROW_UP_LONG;
    ToolTip = "Paint Ceiling";
}

SetDoorTool::SetDoorTool()
{
    Icon = ICON_FA_DOOR_OPEN;
    ToolTip = "Set Door";
}

SetObjectTool::SetObjectTool()
{
    Icon = ICON_FA_PERSON;
    ToolTip = "Set Object";
}
