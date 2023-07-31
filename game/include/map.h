#pragma once


#include <stdint.h>
#include <vector>

#include "raymath.h"

struct EntityLocation
{
	Vector2 Position = { 0 };
	Vector2 Facing = { 0 };

	inline float GetFacingAngle() { return atan2f(Facing.y, Facing.x) * RAD2DEG; }
};

class Map
{
public:
    Map();

    uint8_t GetCell(int x, int y) const;

    inline size_t GetWidth() const { return Width; }
    inline size_t GetHeight() const { return Height; }

    inline int GetCellIndex(int x, int y) const { return y * int(Width) + x; }

    inline void GetCellXY(size_t index, int& x, int& y) const
    {
        y = int(index / Width);
        x = int(index % Width);
    }

protected:
    size_t Width = 0;
    size_t Height = 0;
    std::vector<uint8_t> Cells;
};
