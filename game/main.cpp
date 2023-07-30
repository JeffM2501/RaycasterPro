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
Vector2 CameraPlane = { 0, -0.66f };	// the 2d equivalent of a camera plane, rotates with the player

void SetupCameraPlane()
{
    float fovX = GetFOVX(ViewFOVY);
    CameraPlane.y = -tanf(fovX * DEG2RAD * 0.5f);
    CameraPlane.x = 0;
}

// some stats
int CastCount = 0;
int FaceCount = 0;

// used to know what side of a grid was hit
enum class HitNormals : uint8_t
{
    North = 0,
    South,
    East,
    West
};

// a ray that has been cast, with cached info
struct RayResult
{
    // the ray's direction
    Vector2 Directon = { 0 };

    // the distance to the cell hit
    float Distance = 0;

    // the side of the grid that was hit
    HitNormals Normal;

    // what kind of grid cell was hit
    uint8_t HitGridType = 0;

    int HitCellIndex = -1;
};

// the rays that make up the view. This is a fixed size array based on the render view's width (one for each pixel in X)
std::vector<RayResult> RaySet;

// get the grid for map coordinate
uint8_t GetMapGrid(uint8_t x, uint8_t y)
{
    return WorldMap.GetCell(x,y);
}

uint8_t GetMapGrid(const Vector2& pos)
{
    return GetMapGrid(uint8_t(pos.x), uint8_t(pos.y));
}

// cast a ray and find out what it hits
void CastRay(RayResult& ray)
{
    ray.Distance = -1;

    // The current grid point we are in
    int mapX = int(floor(PlayerPos.x));
    int mapY = int(floor(PlayerPos.y));

    //length of ray from current position to next x or y-side
    float sideDistX = 0;
    float sideDistY = 0;

    // length of ray from one x or y-side to next x or y-side
    // these are derived as:
    // deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
    // deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
    // which can be simplified to abs(|rayDir| / rayDirX) and abs(|rayDir| / rayDirY)
    // where |rayDir| is the length of the vector (rayDirX, rayDirY). Its length,
    // unlike (dirX, dirY) is not 1, however this does not matter, only the
    // ratio between deltaDistX and deltaDistY matters, due to the way the DDA
    // stepping further below works. So the values can be computed as below.
    // Division through zero is prevented, even though technically that's not
    // needed in C++ with IEEE 754 floating point values.
    float deltaDistX = (ray.Directon.x == 0) ? float(1e30) : float(fabs(1.0f / ray.Directon.x));
    float deltaDistY = (ray.Directon.y == 0) ? float(1e30) : float(fabs(1.0f / ray.Directon.y));

    float perpWallDist = 0;

    // what direction to step in x or y-direction (either +1 or -1)
    int stepX = 0;
    int stepY = 0;

    bool hit = false; //was there a wall hit?
    bool side = false; //was a NS or a EW wall hit?

    // calculate step and initial sideDist
    if (ray.Directon.x < 0)
    {
        stepX = -1;
        sideDistX = (PlayerPos.x - mapX) * deltaDistX;
    }
    else
    {
        stepX = 1;
        sideDistX = (mapX + 1.0f - PlayerPos.x) * deltaDistX;
    }

    if (ray.Directon.y < 0)
    {
        stepY = -1;
        sideDistY = (PlayerPos.y - mapY) * deltaDistY;
    }
    else
    {
        stepY = 1;
        sideDistY = (mapY + 1.0f - PlayerPos.y) * deltaDistY;
    }

    // perform DDA Digital Differential Analyzer to walk the line
    while (!hit)
    {
        //jump to next map square, either in x-direction, or in y-direction
        if (sideDistX < sideDistY)
        {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = false;
        }
        else
        {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = true;
        }

        if (mapX >= WorldMap.GetWidth() || mapX < 0 || mapY >= WorldMap.GetHeight() || mapY < 0)
            break;

        ray.HitGridType = WorldMap.GetCell(mapX, mapY);
        ray.HitCellIndex = WorldMap.GetCellIndex(mapX, mapY);

        //Check if ray has hit a wall
        if (ray.HitGridType != 0)
            hit = true;

        WorldMap.SetCellVis(mapX, mapY);
    }

    if (!hit)
    {
        ray.Distance = -1;
        return;
    }


    // Calculate distance projected on camera direction. This is the shortest distance from the point where the wall is
    // hit to the camera plane. Euclidean to center camera point would give fisheye effect!
    // This can be computed as (mapX - posX + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
    // for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
    // because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
    // steps, but we subtract deltaDist once because one step more into the wall was taken above.
    if (!side)
    {
        perpWallDist = (sideDistX - deltaDistX);
        ray.Normal = stepX < 0 ? HitNormals::East : HitNormals::West;
    }
    else
    {
        perpWallDist = (sideDistY - deltaDistY);
        ray.Normal = stepY < 0 ? HitNormals::North : HitNormals::South;
    }

    ray.Distance = perpWallDist;
}

bool CastRayPair(int minPixel, int maxPixel)
{
    float cameraX = 0;

    RayResult& minRay = RaySet[minPixel];
    RayResult& maxRay = RaySet[maxPixel];

    // we've been here before
    if (minRay.HitCellIndex >= 0 && maxRay.HitCellIndex >= 0 && maxPixel - minPixel <= 1)
        return true;

    if (minRay.HitCellIndex < 0)
    {
        cameraX = 2 * minPixel / (float)GetScreenWidth() - 1; //x-coordinate in camera space
        minRay.Directon.x = PlayerFacing.x + CameraPlane.x * cameraX;
        minRay.Directon.y = PlayerFacing.y + CameraPlane.y * cameraX;
        CastRay(minRay);
    }

    if (maxRay.HitCellIndex < 0)
    {
        cameraX = 2 * maxPixel / (float)GetScreenWidth() - 1; //x-coordinate in camera space
        maxRay.Directon.x = PlayerFacing.x + CameraPlane.x * cameraX;
        maxRay.Directon.y = PlayerFacing.y + CameraPlane.y * cameraX;

        CastRay(maxRay);
    }

    return minRay.HitCellIndex == maxRay.HitCellIndex;
}

// compute the rays for the current view
void UpdateRayset()
{
    WorldMap.StartFrame();
    WorldMap.SetCellVis(int(PlayerPos.x), int(PlayerPos.y));

    for (uint16_t i = 0; i < GetScreenWidth(); i++)
        RaySet[i].HitCellIndex = -1;

    size_t index = 0;
    std::vector<std::pair<int, int>> pendingCasts;

    pendingCasts.reserve(GetScreenWidth());
    pendingCasts.emplace_back(0, GetScreenWidth() - 1);

    while (index < pendingCasts.size())
    {
        int min = pendingCasts[index].first;
        int max = pendingCasts[index].second;

        if (!CastRayPair(min, max))
        {
            int bisector = ((max - min) / 2) + min;

            if (min != bisector)
                pendingCasts.emplace_back(min, bisector);

            if (max != bisector)
                pendingCasts.emplace_back(bisector, max);
        }

        index++;
    }

}

// draw the rays in the top view
void DrawRayset(const Vector2& playerPos, float scale)
{
    CastCount = 0;

    for (uint16_t i = 0; i < GetScreenWidth(); i++)
    {
        RayResult& ray = RaySet[i];

        if (ray.Distance >= 0 && ray.HitCellIndex >= 0)
        {
            DrawLineV(playerPos, Vector2Add(playerPos, Vector2Scale(ray.Directon, ray.Distance * scale)), ColorAlpha(SKYBLUE, 0.75f));
            CastCount++;
        }
    }
}

void DrawMapTopView()
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
            bool hit = WorldMap.IsCellVis(x, y);
            if (WorldMap.GetCell(x, y) != 0)
                DrawRectangle(x * MapPixelSize, y * MapPixelSize, MapPixelSize, MapPixelSize, hit ? PURPLE : WHITE);
            else if (hit)
                DrawRectangle(x * MapPixelSize, y * MapPixelSize, MapPixelSize, MapPixelSize, DARKPURPLE);

            DrawRectangleLines(x * MapPixelSize, y * MapPixelSize, MapPixelSize, MapPixelSize, BLACK);
        }
    }

    Vector2 playerPixelSpace = Vector2Scale(PlayerPos, MapPixelSize);

    // draw rays
    DrawRayset(playerPixelSpace, MapPixelSize);

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

// figure out how far from the edge of a cell the current ray is
float GetRayU(const RayResult& ray)
{
    Vector2 target = Vector2Add(PlayerPos, Vector2Scale(ray.Directon, ray.Distance));

    switch (ray.Normal)
    {
    case HitNormals::South:
        return  target.x - floorf(target.x);

    case HitNormals::North:
        return  1.0f - (target.x - floorf(target.x));

    case HitNormals::East:
        return  target.y - floorf(target.y);

    case HitNormals::West:
        return  1.0f - (target.y - floorf(target.y));
    }

    return 0;
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


void DrawView3D()
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

    // fill the map with cells
    int x = -1;
    int y = -1;

    for (const auto& i : WorldMap.GetHitCellCountList())
    {
        WorldMap.GetCellXY(i, x, y);

        uint8_t cellType = WorldMap.GetCell(x, y);

        if (cellType == 0)
        {
            DrawCellFloor(x, y);
            DrawCellCeiling(x, y);
        }
        else
        {
            DrawCellWall(x, y, cellType);
        }
    }

    EndMode3D();
}

// move the player around the map
void UpdateMovement()
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
    CameraPlane = Vector2Rotate(CameraPlane, rotation);

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
    Rectangle sourceRect = { 0,0,GunTexture.width,GunTexture.height };

    float scale = 4;

    Rectangle destRect = { GetScreenWidth() / 2,GetScreenHeight(), GunTexture.width * scale, GunTexture.height * scale };

    Vector2 origin = { (destRect.width / 2) + sinf(GunBobble.x * 5) * 15, destRect.height - 20 + sinf(GunBobble.y * 10) * 10};

    DrawTexturePro(GunTexture, sourceRect, destRect, origin, 0, WHITE);

    DrawTexture(CrosshairTexture, GetScreenWidth() / 2 - CrosshairTexture.width / 2, GetScreenHeight() / 2 - CrosshairTexture.height / 2, ColorAlpha(WHITE, 0.5f));
}

void DrawMiniMap()
{
    Rectangle mapRect = { 0, 0, 300, 300 };
    mapRect.x = GetScreenWidth() - mapRect.width;
    mapRect.y = 0;

    DrawRectangleRec(mapRect, ColorAlpha(BLACK, 0.5f));

    Rectangle destRect = { mapRect.x + mapRect.width/2, mapRect.y + mapRect.height / 2, MapRenderTexture.texture.width, MapRenderTexture.texture.height };

	BeginScissorMode(mapRect.x, mapRect.y, mapRect.width, mapRect.height);

	Rectangle sourceRect = { 0, 0, MapRenderTexture.texture.width, MapRenderTexture.texture.height };
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

    RaySet.resize(GetScreenWidth());
    SetupCameraPlane();
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

    float angle = atan2f(CameraPlane.y, CameraPlane.x) * RAD2DEG;

    // game loop
    while (!WindowShouldClose())
    {
        // move the player
        UpdateMovement();

        // compute the rays for the current view
        // this is where the raycasting happens
        UpdateRayset();

        // update the top view render texture
        DrawMapTopView();

        // Draw the results to the screen
        BeginDrawing();
        ClearBackground(BLACK);

        DrawView3D();

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