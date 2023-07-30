#pragma once

#include "raylib.h"
#include "raycaster.h"

class MiniMap
{
public:
    MiniMap(int size, const Raycaster& raycaster, const Map& map);

    void Unload();

    void SetGridSize(int size);
    inline int GetGridSize() const { return MapPixelSize; }

    void Draw(const Vector2& pos, const Vector2& facing);

protected:
    void Render(const Vector2& pos, const Vector2& facing);
    void DrawRayset(const Vector2& playerPos, float scale);

    const Raycaster& Caster;
    const Map& WorldMap;

    int MapPixelSize = 20;
    float MiniMapSize = 200;

    RenderTexture MapRenderTexture = { 0 };	// render texture for the top view
};
