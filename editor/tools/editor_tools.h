#pragma once

#include "tool_system.h"

constexpr int EraseAction = 1;
constexpr int PaintWallAction = 2;

class SelectTool : public ButtonTool
{
public:
    SelectTool();
};

class EraserTool : public ButtonTool
{
public:
    EraserTool();

    void OnClick(const Vector2& mapCoordinate) override;
};

class PaintWallTool : public ButtonTool
{
public:
    PaintWallTool();
    void OnClick(const Vector2& mapCoordinate) override;
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
    void OnClick(const Vector2& mapCoordinate) override;
};

class SetObjectTool : public ButtonTool
{
public:
    SetObjectTool();
};