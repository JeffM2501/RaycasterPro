#pragma once

#include "tool_system.h"

class SelectTool : public ButtonTool
{
public:
    SelectTool();
};

class EraserTool : public ButtonTool
{
public:
    EraserTool();

    void OnClick(const ImVec2& mapCoordinate) override;
};

class PaintWallTool : public ButtonTool
{
public:
    PaintWallTool();
    void OnClick(const ImVec2& mapCoordinate) override;
};

class PaintFloorTool : public ButtonTool
{
public:
    PaintFloorTool();
};

class PaintCeilingTool : public ButtonTool
{
public:
    PaintCeilingTool();
};

class SetDoorTool : public ButtonTool
{
public:
    SetDoorTool();
};

class SetObjectTool : public ButtonTool
{
public:
    SetObjectTool();
};