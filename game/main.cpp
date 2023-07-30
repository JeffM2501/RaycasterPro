/*
*   Raylib software Raycaster
*   Based on algorithms from
*   https://lodev.org/cgtutor/raycasting.html
*
*   LICENSE: zlib/libpng
*
*   raylib-extras are licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   Copyright (c) 2022 Jeffery Myers (jeffm)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*/

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "map.h"
#include "raycaster.h"

#include <stdint.h>
#include <set>

Map WorldMap;

// how big each map grid is in pixels for the top view
constexpr uint8_t MapPixelSize = 20;

RenderTexture MapRenderTexture;	// render texture for the top view

Texture2D WallTexture = { 0 };
Texture2D GunTexture = { 0 };
Texture2D CrosshairTexture = { 0 };

// 3d view size

//float ViewFOVX = 66.6f;
float ViewFOVY = 40;

Vector2 GunBobble = { 0,0 };

float GetFOVX(float fovY)
{
    float aspectRatio = GetScreenWidth() / (float)GetScreenHeight();

    return 2.0f * atanf(tanf(fovY * DEG2RAD * 0.5f) * aspectRatio) * RAD2DEG;
}

float GetFOVY(float fovX)
{
    float aspectRatio = GetScreenWidth() / (float)GetScreenHeight();
    float halfwidth = tanf(fovX * DEG2RAD / 2);
    return 2 * atanf(halfwidth / aspectRatio) * RAD2DEG;
}

Vector2 PlayerPos = { 4.5f,  2.5f };
Vector2 PlayerFacing = { 1, 0 };

// some stats
int CastCount = 0;
int FaceCount = 0;

// get the grid for map coordinate
uint8_t GetMapGrid(uint8_t x, uint8_t y)
{
    return WorldMap.GetCell(x,y);
}

uint8_t GetMapGrid(const Vector2& pos)
{
    return GetMapGrid(uint8_t(pos.x), uint8_t(pos.y));
}

// compute the rays for the current view

// draw the rays in the top view
void DrawRayset(const Vector2& playerPos, float scale, const Raycaster& raycast)
{
    CastCount = 0;

    for (uint16_t i = 0; i < GetScreenWidth(); i++)
    {
        const RayResult& ray = raycast.GetResults()[i];

        if (ray.Distance >= 0 && ray.HitCellIndex >= 0)
        {
            DrawLineV(playerPos, Vector2Add(playerPos, Vector2Scale(ray.Directon, ray.Distance * scale)), ColorAlpha(SKYBLUE, 0.75f));
            CastCount++;
        }
    }
}

void DrawMapTopView(const Raycaster& raycast)
{
    if (MapPixelSize == 0)
        return;

    BeginTextureMode(MapRenderTexture);
    ClearBackground(DARKGRAY);

    // fill the map with cells
    for (uint8_t y = 0; y < WorldMap.GetHeight(); y++)
    {
        for (uint8_t x = 0; x < WorldMap.GetWidth(); x++)
        {
            bool hit = raycast.IsCellVis(x, y);
            if (WorldMap.GetCell(x, y) != 0)
                DrawRectangle(x * MapPixelSize, y * MapPixelSize, MapPixelSize, MapPixelSize, hit ? PURPLE : WHITE);
            else if (hit)
                DrawRectangle(x * MapPixelSize, y * MapPixelSize, MapPixelSize, MapPixelSize, DARKPURPLE);

            DrawRectangleLines(x * MapPixelSize, y * MapPixelSize, MapPixelSize, MapPixelSize, BLACK);
        }
    }

    Vector2 playerPixelSpace = Vector2Scale(PlayerPos, MapPixelSize);

    // draw rays
    DrawRayset(playerPixelSpace, MapPixelSize, raycast);

    // draw player
    DrawCircleV(playerPixelSpace, MapPixelSize * 0.25f, BLUE);

    // draw forward vector
    Vector2 forwardPixelSpace = Vector2Scale(PlayerFacing, MapPixelSize);

    DrawLineV(playerPixelSpace, Vector2Add(playerPixelSpace, forwardPixelSpace), SKYBLUE);

    // draw axis markers
    DrawLine(MapPixelSize / 4, MapPixelSize / 4, MapPixelSize, MapPixelSize / 4, RED);
    DrawLine(MapPixelSize / 4, MapPixelSize / 4, MapPixelSize / 4, MapPixelSize, GREEN);

    EndTextureMode();
}

// draw the 3d view
void GetCellTypeUs(uint8_t cellType, float& uStart, float& uEnd)
{
    uStart = float(WallTexture.height * (cellType - 1));
    uEnd = uStart + WallTexture.height - 1;

    uStart /= WallTexture.width;
    uEnd /= WallTexture.width;
}

void rlVertex3if(int x, int y, float z)
{
    rlVertex3f(float(x), float(y), z);
}

void DrawCellFloor(int x, int y)
{
    float uStart = 0;
    float uEnd = 1;
    FaceCount++;
    GetCellTypeUs(8, uStart, uEnd);

    rlSetTexture(WallTexture.id);

    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);
    rlNormal3f(0, 0, 1);

    rlTexCoord2f(uStart, 0);
    rlVertex3if(x, y, 0);

    rlTexCoord2f(uEnd, 0);
    rlVertex3if(x+1, y, 0);

    rlTexCoord2f(uEnd, 1);
    rlVertex3if(x + 1, y+1, 0);

    rlTexCoord2f(uStart,1);
    rlVertex3if(x, y+1, 0);

    rlEnd();
}

void DrawCellCeiling(int x, int y)
{
    float uStart = 0;
    float uEnd = 1;
    FaceCount++;
    GetCellTypeUs(8, uStart, uEnd);

    rlSetTexture(WallTexture.id);

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

void DrawCellWall(int x, int y, uint8_t cellType)
{
    float uStart = 0;
    float uEnd = 1;

    GetCellTypeUs(cellType, uStart, uEnd);

    static Color wallColors[4] = { WHITE, Color{128,128,128,255}, Color{196,196,196,255} , Color{200,200,200,255} };

    rlSetTexture(WallTexture.id);
    rlBegin(RL_QUADS);
    Color tint = WHITE;

    if (WorldMap.GetCell(x, y + 1) == 0)
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
    if (WorldMap.GetCell(x, y - 1) == 0)
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
    if (WorldMap.GetCell(x + 1, y) == 0)
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
    if (WorldMap.GetCell(x - 1, y) == 0)
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


void DrawView3D(const Raycaster& raycast)
{
    Camera3D camera = { 0 };
    camera.fovy = ViewFOVY;
    camera.up.z = 1;
    camera.position.x = PlayerPos.x;
    camera.position.y = PlayerPos.y;
    camera.position.z = 0.5f;

    camera.target.x = PlayerPos.x + PlayerFacing.x;
    camera.target.y = PlayerPos.y + PlayerFacing.y;
    camera.target.z = 0.5f;

    BeginMode3D(camera);
    FaceCount = 0;

    for (const auto& pos : raycast.GetHitCelList())
    {
        uint8_t cellType = WorldMap.GetCell(pos.x, pos.y);

        if (cellType == 0)
        {
            DrawCellFloor(pos.x, pos.y);
            DrawCellCeiling(pos.x, pos.y);
        }
        else
        {
            DrawCellWall(pos.x, pos.y, cellType);
        }
    }

    EndMode3D();
}

// move the player around the map
void UpdateMovement(Raycaster& raycaster)
{
    // speeds, based on time
    float rotationSpeed = 180.0f * DEG2RAD * GetFrameTime();
    float movementSpeed = 5.0f * GetFrameTime();

    bool sprint = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    if (sprint)
        movementSpeed *= 2;

    // compute a rotation for this frame
    float rotation = 0;

    if (IsKeyDown(KEY_Q))
        rotation += rotationSpeed;

    if (IsKeyDown(KEY_E))
        rotation -= rotationSpeed;

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        rotation -= GetMouseDelta().x / 100.0f;

    // rotate the player and the camera plane
    PlayerFacing = Vector2Rotate(PlayerFacing, rotation);

    // compute a new position based on movement
    Vector2 newVec = { 0,0 };

    // the vector that is to the left
    Vector2 sideStepVector = { -PlayerFacing.y, PlayerFacing.x };

    // move the new pos based on keys
    if (IsKeyDown(KEY_W))
        newVec = Vector2Add(newVec, Vector2Scale(PlayerFacing, movementSpeed));

    if (IsKeyDown(KEY_S))
        newVec = Vector2Add(newVec, Vector2Scale(PlayerFacing, -movementSpeed));

    if (IsKeyDown(KEY_A))
        newVec = Vector2Add(newVec, Vector2Scale(sideStepVector, movementSpeed));

    if (IsKeyDown(KEY_D))
        newVec = Vector2Add(newVec, Vector2Scale(sideStepVector, -movementSpeed));

    if (Vector2LengthSqr(newVec) > 0)
    {
        GunBobble.y += GetFrameTime() * (sprint ? 2 : 1);
        GunBobble.x += GetFrameTime();
    }

    Vector2 newPos = Vector2Add(PlayerPos, newVec);
    // if the new pos is not inside the world, allow the player to move there
    if (GetMapGrid(newPos) == 0)
        PlayerPos = newPos;
}

void DrawGun()
{
    Rectangle sourceRect = { 0, 0, float(GunTexture.width), float(GunTexture.height) };

    float scale = 4;

    Rectangle destRect = { GetScreenWidth()*0.5f,float(GetScreenHeight()), GunTexture.width * scale, GunTexture.height * scale };

    Vector2 origin = { (destRect.width * 0.5f) + sinf(GunBobble.x * 5) * 15, destRect.height - 20 + sinf(GunBobble.y * 10) * 10};

    DrawTexturePro(GunTexture, sourceRect, destRect, origin, 0, WHITE);

    DrawTexture(CrosshairTexture, GetScreenWidth()/2 - CrosshairTexture.width/2, GetScreenHeight()/2 - CrosshairTexture.height/2, ColorAlpha(WHITE, 0.5f));
}

void DrawMiniMap()
{
    Rectangle mapRect = { 0, 0, 300, 300 };
    mapRect.x = GetScreenWidth() - mapRect.width;
    mapRect.y = 0;

    DrawRectangleRec(mapRect, ColorAlpha(BLACK, 0.5f));

    Rectangle destRect = { mapRect.x + mapRect.width * 0.5f, mapRect.y + mapRect.height * 0.5f, float(MapRenderTexture.texture.width), float(MapRenderTexture.texture.height) };

	BeginScissorMode(int(mapRect.x), int(mapRect.y), int(mapRect.width), int(mapRect.height));

	Rectangle sourceRect = { 0, 0, float(MapRenderTexture.texture.width), float(MapRenderTexture.texture.height) };
	Vector2 offset = { (PlayerPos.x * MapPixelSize), MapRenderTexture.texture.height - (PlayerPos.y * MapPixelSize) };
 
 	// Note that this render texture is NOT flipped in Y, so that the view has Y be up not down
 	DrawTexturePro(MapRenderTexture.texture, sourceRect, destRect, offset, 0, WHITE);
    EndScissorMode();
}

int main()
{
    // set up the window
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1600, 900, "RaycasterPro Example");
    SetTargetFPS(350);

    Raycaster raycaster(WorldMap, GetScreenWidth(), GetFOVX(ViewFOVY));

    // load render textures for the top view and 3d view
    MapRenderTexture = LoadRenderTexture(int(WorldMap.GetWidth() * MapPixelSize), int(WorldMap.GetHeight() * MapPixelSize));

    // textures for our walls
    WallTexture = LoadTexture("resources/textures/textures.png");
    GenTextureMipmaps(&WallTexture);
    SetTextureFilter(WallTexture, TEXTURE_FILTER_ANISOTROPIC_16X);

    // texture for the gun
    GunTexture = LoadTexture("resources/textures/gun.png");
    SetTextureFilter(WallTexture, TEXTURE_FILTER_POINT);

    CrosshairTexture = LoadTexture("resources/textures/crosshair.png");
    // game loop
    while (!WindowShouldClose())
    {
        // move the player
        UpdateMovement(raycaster);

        // compute the rays for the current view
        // this is where the raycasting happens
        raycaster.StartFrame(PlayerPos, PlayerFacing);
      //  raycaster.UpdateRayset();

        // update the top view render texture
        DrawMapTopView(raycaster);

        // Draw the results to the screen
        BeginDrawing();
        ClearBackground(BLACK);

        DrawView3D(raycaster);

        DrawGun();

        DrawMiniMap();

        // text overlay
        DrawRectangle(0, 0, 450, 50, ColorAlpha(BLACK, 0.25f));
        DrawFPS(2, 0);
        DrawText(TextFormat("Player X%2.1f, X%2.1f, Casts %d Faces = %d", PlayerPos.x, PlayerPos.y, CastCount, FaceCount), 2, 20, 20, WHITE);

        EndDrawing();
    }

    // cleanup
    UnloadTexture(WallTexture);
    UnloadRenderTexture(MapRenderTexture);

    CloseWindow();
    return 0;
}