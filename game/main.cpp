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

#include "map.h"
#include "raycaster.h"
#include "mini_map.h"
#include "view_render.h"

#include <stdint.h>
#include <set>

Map WorldMap;

// how big each map grid is in pixels for the top view
constexpr uint8_t MapPixelSize = 20;


Texture2D GunTexture = { 0 };
Texture2D CrosshairTexture = { 0 };

//float ViewFOVX = 66.6f;
float ViewFOVY = 40;

Vector2 GunBobble = { 0,0 };

float GetFOVX(float fovY)
{
    float aspectRatio = GetScreenWidth() / (float)GetScreenHeight();

    return 2.0f * atanf(tanf(fovY * DEG2RAD * 0.5f) * aspectRatio) * RAD2DEG;
}

Vector2 PlayerPos = { 4.5f,  2.5f };
Vector2 PlayerFacing = { 1, 0 };

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
    if (WorldMap.GetCell(int(newPos.x),int(newPos.y)) == 0)
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

int main()
{
    // set up the window
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1600, 900, "RaycasterPro Example");
    SetTargetFPS(350);

    Raycaster raycaster(WorldMap, GetScreenWidth(), GetFOVX(ViewFOVY));
    MiniMap miniMap(20, raycaster, WorldMap);
    ViewRenderer renderer(raycaster, WorldMap);
    renderer.SetFOVY(ViewFOVY);

    // texture for the gun
    GunTexture = LoadTexture("resources/textures/gun.png");

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

        // Draw the results to the screen
        BeginDrawing();
        ClearBackground(BLACK);

        renderer.Draw(PlayerPos, PlayerFacing);

        DrawGun();

        miniMap.Draw(PlayerPos,PlayerFacing);

        // text overlay
        DrawRectangle(0, 0, 450, 50, ColorAlpha(BLACK, 0.25f));
        DrawFPS(2, 0);
        DrawText(TextFormat("Player X%2.1f, X%2.1f, Casts %d Faces = %d", PlayerPos.x, PlayerPos.y, raycaster.GetCastCount(), renderer.GetFaceCount()), 2, 20, 20, WHITE);

        EndDrawing();
    }

    // cleanup
    UnloadTexture(GunTexture);
    miniMap.Unload();
    renderer.Unload();

    CloseWindow();
    return 0;
}