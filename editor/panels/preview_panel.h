#pragma once

#include "panels/panel.h"

#include "view_render.h"

#include "raylib.h"

class PreviewPanel : public Panel
{
public:
    PreviewPanel();

protected:
    void OnShow() override;

    RenderTexture PreviewTexture = { 0 };
    
    Raycaster Caster;
    ViewRenderer Renderer;
};