#include "raycaster.h"

Raycaster::Raycaster(const Map* map, int renderWidth, float renderFOV)
    : WorldMap(map)
    , RenderWidth(renderWidth)
    , RenderFOVX(renderFOV)
{
    NominalCameraPlane.y = -tanf(renderFOV * DEG2RAD * 0.5f);
    NominalCameraPlane.x = 0;

    RaySet.resize(renderWidth);

    SetMap(map);
}

void Raycaster::SetMap(const Map* map)
{
    if (map)
    {
        WorldMap = map;
        CellStatus.resize(WorldMap->GetWidth() * WorldMap->GetHeight());
        CellStatus.assign(CellStatus.size(), 0);
    }
}

void Raycaster::StartFrame(const EntityLocation& loc)
{
    // set the camera plane for this view
    float angle = atan2f(loc.Facing.y, loc.Facing.x);
    CameraPlane = Vector2Rotate(NominalCameraPlane, angle);

    // clear any previous hit cells
    for (const auto& i : HitCells)
        CellStatus[i] = 0;

    HitCells.clear();
    HitCellLocs.clear();

    CastCount = 0;

    // cast this frame
    UpdateRayset(loc);
}

// cast a ray and find out what it hits
void Raycaster::CastRay(RayResult& ray, const Vector2& pos)
{
    ray.Distance = -1;
    if (!WorldMap)
        return;

    CastCount++;

    // The current grid point we are in
    int mapX = int(floor(pos.x));
    int mapY = int(floor(pos.y));

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
        sideDistX = (pos.x - mapX) * deltaDistX;
    }
    else
    {
        stepX = 1;
        sideDistX = (mapX + 1.0f - pos.x) * deltaDistX;
    }

    if (ray.Directon.y < 0)
    {
        stepY = -1;
        sideDistY = (pos.y - mapY) * deltaDistY;
    }
    else
    {
        stepY = 1;
        sideDistY = (mapY + 1.0f - pos.y) * deltaDistY;
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

        if (mapX >= WorldMap->GetWidth() || mapX < 0 || mapY >= WorldMap->GetHeight() || mapY < 0)
            break;

        ray.HitGridType = WorldMap->GetCell(mapX, mapY);
        ray.HitCellIndex = WorldMap->GetCellIndex(mapX, mapY);
        ray.TargetCell.x = mapX;
        ray.TargetCell.y = mapY;

        //Check if ray has hit a wall
        if (ray.HitGridType != 0)
            hit = true;

        SetCellVis(mapX, mapY);
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

bool Raycaster::CastRayPair(int minPixel, int maxPixel, const EntityLocation& loc)
{
    float cameraX = 0;

    RayResult& minRay = RaySet[minPixel];
    RayResult& maxRay = RaySet[maxPixel];

    // we've been here before
    if (minRay.HitCellIndex >= 0 && maxRay.HitCellIndex >= 0 && maxPixel - minPixel <= 1)
        return true;

    if (minRay.HitCellIndex < 0)
    {
        cameraX = 2 * minPixel / (float)RenderWidth - 1; //x-coordinate in camera space
        minRay.Directon.x = loc.Facing.x + CameraPlane.x * cameraX;
        minRay.Directon.y = loc.Facing.y + CameraPlane.y * cameraX;
        CastRay(minRay, loc.Position);
    }

    if (maxRay.HitCellIndex < 0)
    {
        cameraX = 2 * maxPixel / (float)RenderWidth - 1; //x-coordinate in camera space
        maxRay.Directon.x = loc.Facing.x + CameraPlane.x * cameraX;
        maxRay.Directon.y = loc.Facing.y + CameraPlane.y * cameraX;

        CastRay(maxRay, loc.Position);
    }

    return minRay.HitCellIndex == maxRay.HitCellIndex;
}

void Raycaster::UpdateRayset(const EntityLocation& loc)
{
    SetCellVis(int(loc.Position.x), int(loc.Position.y));

    for (uint16_t i = 0; i < RenderWidth; i++)
        RaySet[i].HitCellIndex = -1;

    size_t index = 0;
    std::vector<std::pair<int, int>> pendingCasts;

    pendingCasts.reserve(RenderWidth);
    pendingCasts.emplace_back(0, RenderWidth - 1);

    while (index < pendingCasts.size())
    {
        int min = pendingCasts[index].first;
        int max = pendingCasts[index].second;

        if (!CastRayPair(min, max, loc))
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

bool Raycaster::IsCellVis(int x, int y) const
{
    if (!WorldMap)
        return false;

    int index = y * (int)WorldMap->GetWidth() + x;
    return CellStatus[index] == 1;
}

void Raycaster::SetCellVis(int x, int y)
{
    if (!WorldMap)
        return;

    int index = y * (int)WorldMap->GetWidth() + x;
    uint8_t& id = CellStatus[index];
    if (id == 1)
        return;

    id = 1;
    HitCells.push_back(index);
    HitCellLocs.emplace_back(Vector2i{ x, y });
}
