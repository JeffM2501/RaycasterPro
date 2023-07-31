#pragma once

#include "raylib.h"
#include "raycaster.h"

class ViewRenderer
{
public:
    ViewRenderer(const Raycaster& raycaster, const Map& map);

    void Unload();

    void Draw(const EntityLocation& loc);

    inline void SetFOVY(float fov) { ViewCamera.fovy = fov; }
    inline float GetFOVY() const { return ViewCamera.fovy; }

    inline int GetFaceCount() const { return FaceCount; }

protected:

    void DrawCellFloor(int x, int y);
    void DrawCellCeiling(int x, int y);
    void DrawCellWall(int x, int y, uint8_t cellType);

    void GetCellTypeUs(uint8_t cellType, float& uStart, float& uEnd);
       
    const Raycaster& Caster;
    const Map& WorldMap;

    Texture2D MapTiles = { 0 };

    Camera3D ViewCamera = { 0 };

    int FaceCount = 0;
};
