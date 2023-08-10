#include "tools/editor_tools.h"
#include "editor.h"
#include "raylib.h"

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

void EraserTool::OnClick(const Vector2& mapCoordinate)
{
    Editor::GetActiveEditor().SetCell(mapCoordinate, CellState::Empty, 0, EraseAction);
}

PaintWallTool::PaintWallTool()
{
    Icon = ICON_FA_PAINT_ROLLER;
    ToolTip = "Paint Walls";
}

void PaintWallTool::OnClick(const Vector2& mapCoordinate)
{
    Editor::GetActiveEditor().SetCell(mapCoordinate, CellState::Solid, Editor::GetActiveEditor().GetCurrentMaterial(), PaintWallAction);
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

void SetDoorTool::OnClick(const Vector2& mapCoordinate)
{
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        return;

    auto& editor = Editor::GetActiveEditor();

    Editor::GetActiveEditor().SetCell(mapCoordinate, CellState::Door, Editor::GetActiveEditor().GetCurrentMaterial(), PaintWallAction);

    // set current cell in inspector
}

SetObjectTool::SetObjectTool()
{
    Icon = ICON_FA_PERSON;
    ToolTip = "Set Object";
}
