#include "mini_map.h"

MiniMap::MiniMap(int size, const Raycaster& raycaster, const Map& map)
    : Caster(raycaster)
    , WorldMap(map)
{
    SetGridSize(size);
}


void MiniMap::Unload()
{
    if (MapRenderTexture.id != 0)
        UnloadRenderTexture(MapRenderTexture);

    if (MapTileCache.id != 0)
        UnloadRenderTexture(MapTileCache);

    MapRenderTexture.id = 0;
    MapTileCache.id = 0;
}

void MiniMap::SetGridSize(int size)
{
    MapPixelSize = size;

    Unload();

    MapRenderTexture = LoadRenderTexture(int(WorldMap.GetWidth()) * MapPixelSize, int(WorldMap.GetHeight()) * MapPixelSize);

    MapTileCache = LoadRenderTexture(int(WorldMap.GetWidth()) * MapPixelSize, int(WorldMap.GetHeight()) * MapPixelSize);
    BeginTextureMode(MapTileCache);
    ClearBackground(DARKGRAY);
    // cache off the fixed part of the map so we don't have to draw it every time
    for (uint8_t y = 0; y < WorldMap.GetHeight(); y++)
    {
        for (uint8_t x = 0; x < WorldMap.GetWidth(); x++)
        {
            if (WorldMap.GetCellSolid(x, y))
                DrawRectangle(x * MapPixelSize, y * MapPixelSize, MapPixelSize, MapPixelSize, WHITE);
            DrawRectangleLines(x * MapPixelSize, y * MapPixelSize, MapPixelSize, MapPixelSize, BLACK);
        }
    }
    EndTextureMode();
}

void MiniMap::Draw(const EntityLocation& loc)
{
    Render(loc);

    Rectangle mapRect = { 0, 0, 300, 300 };
    mapRect.x = GetScreenWidth() - mapRect.width;
    mapRect.y = 0;

    DrawRectangleRec(mapRect, ColorAlpha(BLACK, 0.5f));

    Rectangle destRect = { mapRect.x + mapRect.width * 0.5f, mapRect.y + mapRect.height * 0.5f, float(MapRenderTexture.texture.width), float(MapRenderTexture.texture.height) };

    BeginScissorMode(int(mapRect.x), int(mapRect.y), int(mapRect.width), int(mapRect.height));

    Rectangle sourceRect = { 0, 0, float(MapRenderTexture.texture.width), float(MapRenderTexture.texture.height) };
    Vector2 offset = { (loc.Position.x * MapPixelSize), MapRenderTexture.texture.height - (loc.Position.y * MapPixelSize) };

    // Note that this render texture is NOT flipped in Y, so that the view has Y be up not down
    DrawTexturePro(MapRenderTexture.texture, sourceRect, destRect, offset, 0, WHITE);
    EndScissorMode();
}

void MiniMap::DrawRayset(const Vector2& playerPos, float scale)
{
    for (const RayResult& ray : Caster.GetResults())
    {
        if (ray.Distance >= 0 && ray.HitCellIndex >= 0)
            DrawLineV(playerPos, Vector2Add(playerPos, Vector2Scale(ray.Directon, ray.Distance * scale)), ColorAlpha(SKYBLUE, 0.75f));
    }
}

void MiniMap::Render(const EntityLocation& loc)
{
    BeginTextureMode(MapRenderTexture);
    ClearBackground(DARKGRAY);

    DrawTextureRec(MapTileCache.texture, Rectangle{ 0,0,float(MapTileCache.texture.width),float(-MapTileCache.texture.height) }, Vector2Zero(), WHITE);

    for (const auto& cell : Caster.GetHitCelList())
    {
        if (WorldMap.GetCellSolid(cell.x, cell.y))
            DrawRectangle(cell.x * MapPixelSize, cell.y * MapPixelSize, MapPixelSize, MapPixelSize, ColorAlpha(PURPLE, 0.5f));
        else
            DrawRectangle(cell.x * MapPixelSize, cell.y * MapPixelSize, MapPixelSize, MapPixelSize, ColorAlpha(DARKPURPLE, 0.5f));
    }

    Vector2 playerPixelSpace = Vector2Scale(loc.Position, float(MapPixelSize));

    // draw rays
    DrawRayset(playerPixelSpace, float(MapPixelSize));

    // draw player
    DrawCircleV(playerPixelSpace, MapPixelSize * 0.25f, BLUE);

    // draw forward vector
    Vector2 forwardPixelSpace = Vector2Scale(loc.Facing, float(MapPixelSize));

    DrawLineV(playerPixelSpace, Vector2Add(playerPixelSpace, forwardPixelSpace), SKYBLUE);

    // draw axis markers
    DrawLine(MapPixelSize / 4, MapPixelSize / 4, MapPixelSize, MapPixelSize / 4, RED);
    DrawLine(MapPixelSize / 4, MapPixelSize / 4, MapPixelSize / 4, MapPixelSize, GREEN);

    EndTextureMode();
}