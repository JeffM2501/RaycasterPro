#pragma once

#include "map.h"
#include "raymath.h"

// used to know what side of a grid was hit
enum class HitNormals : uint8_t
{
    North = 0,
    South,
    East,
    West
};

struct Vector2i
{
    int x = -1;
    int y = -1;
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
    Vector2i TargetCell;
};

class Raycaster
{
public:
    Raycaster(Map& map, int renderWidth, float renderFOV);

    void StartFrame(const EntityLocation& loc);

    inline const std::vector<RayResult>& GetResults() const { return RaySet; }
    inline const std::vector<Vector2i>& GetHitCelList() const { return HitCellLocs; }

    bool IsCellVis(int x, int y) const;

    inline int GetCastCount() const { return CastCount; }

protected:
    void CastRay(RayResult& ray, const Vector2& pos);

    bool CastRayPair(int minPixel, int maxPixel, const EntityLocation& loc);

    void UpdateRayset(const EntityLocation& loc);

    void SetCellVis(int x, int y);

    Map& WorldMap;
    int RenderWidth;
    float RenderFOVX;

    int CastCount = 0;

    std::vector<RayResult> RaySet;
    Vector2 CameraPlane;
    Vector2 NominalCameraPlane;

    std::vector<uint8_t> CellStatus;
    std::vector<size_t> HitCells;
    std::vector<Vector2i> HitCellLocs;
};