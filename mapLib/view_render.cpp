
#include "view_render.h"
#include "rlgl.h"

ViewRenderer::ViewRenderer(const Raycaster& raycaster, const Map* map)
    : Caster(raycaster)
    , WorldMap(map)
{
    ViewCamera.fovy = 40;
    ViewCamera.up.z = 1;
}

void ViewRenderer::SetTileTexture(const Texture2D& texture)
{
    MapTiles = texture;
    if (MapTiles.mipmaps != 0)
    {
        GenTextureMipmaps(&MapTiles);
        SetTextureFilter(MapTiles, TEXTURE_FILTER_ANISOTROPIC_4X);
    }
}

void ViewRenderer::SetMap(const Map* map)
{
    WorldMap = map;
}

void ViewRenderer::Unload()
{
    UnloadTexture(MapTiles);
}

void ViewRenderer::Draw(const EntityLocation& loc)
{
    if (!WorldMap)
        return;

    ViewCamera.position.x = loc.Position.x;
    ViewCamera.position.y = loc.Position.y;
    ViewCamera.position.z = 0.5f;

    ViewCamera.target.x = loc.Position.x + loc.Facing.x;
    ViewCamera.target.y = loc.Position.y + loc.Facing.y;
    ViewCamera.target.z = 0.5f;

    BeginMode3D(ViewCamera);
    FaceCount = 0;

    for (const auto& pos : Caster.GetHitCelList())
    {
        if (WorldMap->GetCellSolid(pos.x,pos.y) == 0)
        {
            DrawCellFloor(pos.x, pos.y);
            DrawCellCeiling(pos.x, pos.y);
        }
        else
        {
            DrawCellWall(pos.x, pos.y, WorldMap->GetCellTile(pos.x, pos.y));
        }
    }

    EndMode3D();
}

void ViewRenderer::GetCellTypeUs(uint8_t cellType, float& uStart, float& uEnd)
{
    uStart = float(MapTiles.height * (cellType - 1));
    uEnd = uStart + MapTiles.height - 1;

    uStart /= MapTiles.width;
    uEnd /= MapTiles.width;
}

void rlVertex3if(int x, int y, float z)
{
    rlVertex3f(float(x), float(y), z);
}

void ViewRenderer::DrawCellFloor(int x, int y)
{
    float uStart = 0;
    float uEnd = 1;
    FaceCount++;
    GetCellTypeUs(10, uStart, uEnd);

    rlSetTexture(MapTiles.id);

    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);
    rlNormal3f(0, 0, 1);

    rlTexCoord2f(uStart, 0);
    rlVertex3if(x, y, 0);

    rlTexCoord2f(uEnd, 0);
    rlVertex3if(x + 1, y, 0);

    rlTexCoord2f(uEnd, 1);
    rlVertex3if(x + 1, y + 1, 0);

    rlTexCoord2f(uStart, 1);
    rlVertex3if(x, y + 1, 0);

    rlEnd();
}

void ViewRenderer::DrawCellCeiling(int x, int y)
{
    float uStart = 0;
    float uEnd = 1;
    FaceCount++;
    GetCellTypeUs(9, uStart, uEnd);

    rlSetTexture(MapTiles.id);

    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);
    rlNormal3f(0, 0, 1);

    rlTexCoord2f(uStart, 0);
    rlVertex3if(x, y, 1);

    rlTexCoord2f(uStart, 1);
    rlVertex3if(x, y + 1, 1);

    rlTexCoord2f(uEnd, 1);
    rlVertex3if(x + 1, y + 1, 1);

    rlTexCoord2f(uEnd, 0);
    rlVertex3if(x + 1, y, 1);

    rlEnd();
}

void ViewRenderer::DrawCellWall(int x, int y, uint8_t cellType)
{
    if (!WorldMap)
        return;

    float uStart = 0;
    float uEnd = 1;

    GetCellTypeUs(cellType, uStart, uEnd);

    static Color wallColors[4] = { WHITE, Color{128,128,128,255}, Color{196,196,196,255} , Color{200,200,200,255} };

    rlSetTexture(MapTiles.id);
    rlBegin(RL_QUADS);
    Color tint = WHITE;

    if (!WorldMap->GetCellSolid(x, y + 1))
    {
        FaceCount++;
        // north
        tint = wallColors[0];

        rlColor4ub(tint.r, tint.g, tint.b, 255);
        rlNormal3f(0, 1, 0);

        rlTexCoord2f(uStart, 1);
        rlVertex3if(x + 1, y + 1, 0);

        rlTexCoord2f(uEnd, 1);
        rlVertex3if(x, y + 1, 0);

        rlTexCoord2f(uEnd, 0);
        rlVertex3if(x, y + 1, 1);

        rlTexCoord2f(uStart, 0);
        rlVertex3if(x + 1, y + 1, 1);
    }

    // south
    if (!WorldMap->GetCellSolid(x, y - 1))
    {
        FaceCount++;

        tint = wallColors[1];
        rlColor4ub(tint.r, tint.g, tint.b, 255);
        rlNormal3f(0, -1, 0);

        rlTexCoord2f(uStart, 1);
        rlVertex3if(x + 1, y, 0);

        rlTexCoord2f(uStart, 0);
        rlVertex3if(x + 1, y, 1);

        rlTexCoord2f(uEnd, 0);
        rlVertex3if(x, y, 1);

        rlTexCoord2f(uEnd, 1);
        rlVertex3if(x, y, 0);
    }

    // east
    if (!WorldMap->GetCellSolid(x + 1, y))
    {
        FaceCount++;
        tint = wallColors[2];
        rlColor4ub(tint.r, tint.g, tint.b, 255);
        rlNormal3f(1, 0, 0);

        rlTexCoord2f(uStart, 1);
        rlVertex3if(x + 1, y, 0);

        rlTexCoord2f(uEnd, 1);
        rlVertex3if(x + 1, y + 1, 0);

        rlTexCoord2f(uEnd, 0);
        rlVertex3if(x + 1, y + 1, 1);

        rlTexCoord2f(uStart, 0);
        rlVertex3if(x + 1, y, 1);
    }

    // west
    if (!WorldMap->GetCellSolid(x - 1, y))
    {
        FaceCount++;
        tint = wallColors[3];
        rlColor4ub(tint.r, tint.g, tint.b, 255);
        rlNormal3f(-1, 0, 0);

        rlTexCoord2f(uStart, 1);
        rlVertex3if(x, y, 0);

        rlTexCoord2f(uStart, 0);
        rlVertex3if(x, y, 1);

        rlTexCoord2f(uEnd, 0);
        rlVertex3if(x, y + 1, 1);

        rlTexCoord2f(uEnd, 1);
        rlVertex3if(x, y + 1, 0);
    }

    rlEnd();
}
