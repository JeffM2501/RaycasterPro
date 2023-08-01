#pragma once

#include <stdint.h>
#include <vector>

struct Vector2i
{
    int x = -1;
    int y = -1;
};

class Map
{
public:
    Map();
    Map(const std::vector<uint8_t>& cells, int width, int height);

    uint8_t GetCell(int x, int y) const;
    uint8_t GetCell(const Vector2i& location) const { return GetCell(location.x, location.y); }

    inline int GetWidth() const { return Width; }
    inline int GetHeight() const { return Height; }

    inline int GetCellIndex(int x, int y) const { return y * int(Width) + x; }
    inline int GetCellIndex(const Vector2i& location) const { return location.y * int(Width) + location.x; }

    inline const std::vector<uint8_t>& GetCells() const { return Cells; }

    inline void GetCellXY(size_t index, int& x, int& y) const
    {
        y = int(index / Width);
        x = int(index % Width);
    }

    inline Vector2i GetCellLocation(size_t index) const
    {
        return Vector2i{int(index / Width), int(index % Width)};
    }

protected:
    int Width = 0;
    int Height = 0;
    std::vector<uint8_t> Cells;
};
