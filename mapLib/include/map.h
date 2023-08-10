#pragma once

#include <stdint.h>
#include <vector>

struct Vector2i
{
    int x = -1;
    int y = -1;

    Vector2i() = default;
    Vector2i(int _x, int _y) : x(_x), y(_y) {}
    Vector2i(const Vector2i& v)
    {
        x = v.x;
        y = v.y;
    }
};

enum class CellState : uint8_t
{
	Empty = 0,
	Solid = 1,
    Door = 2,
};

enum class DoorFlags : uint8_t
{
    None = 0,
    YOrient = 0x01,
    SplitOpen = 0x02,
    PosOpen = 0x04,
};

struct MapCell
{
    CellState State = CellState::Empty;
    uint8_t Tile = 1;
    uint8_t Flags = 0;
};

class Map
{
public:
    Map();

    bool GetCellSolid(int x, int y) const;
    void SetCellState(int x, int y, CellState state);

    uint8_t GetCellTile(int x, int y) const;
    void SetCellTile(int x, int y, uint8_t tile);

	uint8_t GetCellFlags(int x, int y) const;
	void SetCellFlags(int x, int y, uint8_t flags);

    bool GetCellPassable(int x, int y) const;

    inline int GetWidth() const { return Width; }
    inline int GetHeight() const { return Height; }

    inline int GetCellIndex(int x, int y) const { return y * int(Width) + x; }
    inline int GetCellIndex(const Vector2i& location) const { return location.y * int(Width) + location.x; }

    inline void GetCellXY(size_t index, int& x, int& y) const
    {
        y = int(index / Width);
        x = int(index % Width);
    }

    inline Vector2i GetCellLocation(size_t index) const
    {
        return Vector2i(int(index / Width), int(index % Width));
    }

    inline std::vector<MapCell>& GetCellsList() { return Cells; }

    void Resize(int newWidth, int newHeight);

protected:

    int Width = 0;
    int Height = 0;
    std::vector<MapCell> Cells;
};
