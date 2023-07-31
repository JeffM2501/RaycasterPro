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
#include "map_collider.h"

#include <stdint.h>
#include <set>

Map WorldMap;
constexpr float ViewFOVY = 40;

// HUD info
Texture2D GunTexture = { 0 };
Texture2D CrosshairTexture = { 0 };
Vector2 GunBobble = { 0,0 };

bool UseButtonForMouse = true;

bool SearchAndSetResourceDir(const char* folderName)
{
    // check the working dir
    if (DirectoryExists(folderName))
    {
        ChangeDirectory(TextFormat("%s/%s", GetWorkingDirectory(), folderName));
        return true;
    }

    const char* appDir = GetApplicationDirectory();

    // check the applicationDir
    const char* dir = TextFormat("%s%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    // check one up from the app dir
    dir = TextFormat("%s../%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    // check two up from the app dir
    dir = TextFormat("%s../../%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    // check three up from the app dir
    dir = TextFormat("%s../../../%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    return false;
}

float GetFOVX(float fovY)
{
    float aspectRatio = GetScreenWidth() / (float)GetScreenHeight();

    return 2.0f * atanf(tanf(fovY * DEG2RAD * 0.5f) * aspectRatio) * RAD2DEG;
}

// player data
EntityLocation Player{ Vector2{ 4.5f,  2.5f }, Vector2{1, 0} };

// move the player around the map
void UpdateMovement(MapCollider& collider)
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

    if (!UseButtonForMouse || IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        rotation -= GetMouseDelta().x / 100.0f;

    // rotate the player and the camera plane
    Player.Facing = Vector2Rotate(Player.Facing, rotation);

    // compute a new position based on movement
    Vector2 newVec = { 0,0 };

    // the vector that is to the left
    Vector2 sideStepVector = { -Player.Facing.y, Player.Facing.x };

    // move the new pos based on keys
    if (IsKeyDown(KEY_W))
        newVec = Vector2Add(newVec, Vector2Scale(Player.Facing, movementSpeed));

    if (IsKeyDown(KEY_S))
        newVec = Vector2Add(newVec, Vector2Scale(Player.Facing, -movementSpeed));

    if (IsKeyDown(KEY_A))
        newVec = Vector2Add(newVec, Vector2Scale(sideStepVector, movementSpeed));

    if (IsKeyDown(KEY_D))
        newVec = Vector2Add(newVec, Vector2Scale(sideStepVector, -movementSpeed));

    if (Vector2LengthSqr(newVec) > 0)
    {
        GunBobble.y += GetFrameTime() * (sprint ? 2 : 1);
        GunBobble.x += GetFrameTime();
    }

    collider.Move(Player, newVec, 0.25f);
}

void DrawGun()
{
    Rectangle sourceRect = { 0, 0, float(GunTexture.width), float(GunTexture.height) };

    float scale = 3.5f;

    Rectangle destRect = { GetScreenWidth()*0.5f,float(GetScreenHeight()), GunTexture.width * scale, GunTexture.height * scale };

    Vector2 origin = { (destRect.width * 0.5f) + sinf(GunBobble.x * 5) * 15, destRect.height - 20 + sinf(GunBobble.y * 10) * 10};

    DrawTexturePro(GunTexture, sourceRect, destRect, origin, 0, WHITE);

    DrawTexture(CrosshairTexture, GetScreenWidth()/2 - CrosshairTexture.width/2, GetScreenHeight()/2 - CrosshairTexture.height/2, ColorAlpha(WHITE, 0.5f));
}

int main()
{
    SearchAndSetResourceDir("resources");
    // set up the window
  
    int width = 1600;
    int height = 900;

#ifndef _DEBUG
    width = 0;
    height = 0;
    SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
#endif

    InitWindow(width, height, "RaycasterPro Example");
    SetTargetFPS(250);

#ifndef _DEBUG
    DisableCursor();
    UseButtonForMouse = false;
#endif

    Raycaster raycaster(WorldMap, GetScreenWidth(), GetFOVX(ViewFOVY));
    MiniMap miniMap(20, raycaster, WorldMap);
    ViewRenderer renderer(raycaster, WorldMap);
    MapCollider collider(WorldMap);

    renderer.SetFOVY(ViewFOVY);

    // texture for the gun
    GunTexture = LoadTexture("textures/gun.png");

    CrosshairTexture = LoadTexture("textures/crosshair.png");
    // game loop
    while (!WindowShouldClose())
    {

        if (IsKeyPressed(KEY_PAGE_UP))
            miniMap.SetGridSize(miniMap.GetGridSize() + 1);
		if (IsKeyPressed(KEY_PAGE_DOWN) && miniMap.GetGridSize() > 1)
			miniMap.SetGridSize(miniMap.GetGridSize() - 1);


        // move the player
        UpdateMovement(collider);

        raycaster.StartFrame(Player);

        // Draw the results to the screen
        BeginDrawing();
        ClearBackground(BLACK);

        renderer.Draw(Player);

        DrawGun();

        miniMap.Draw(Player);

        // text overlay
        DrawRectangle(0, 0, 450, 50, ColorAlpha(BLACK, 0.25f));
        DrawFPS(2, 0);
        DrawText(TextFormat("Player X%2.1f, X%2.1f, Casts %d Faces = %d", Player.Position.x, Player.Position.y, raycaster.GetCastCount(), renderer.GetFaceCount()), 2, 20, 20, WHITE);

        EndDrawing();
    }

    // cleanup
    UnloadTexture(GunTexture);
    miniMap.Unload();
    renderer.Unload();

    CloseWindow();
    return 0;
}