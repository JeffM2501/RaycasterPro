#pragma once


#include <stdint.h>
#include <vector>

class Map
{
public:
    Map();

    uint8_t GetCell(int x, int y) const;
    bool CellHit(int x, int y) const;

    inline size_t GetWidth() const { return Width; }
    inline size_t GetHeight() const { return Height; }

    inline int GetCellIndex(int x, int y) const { return y * int(Width) + x; }

    void ClearCellStatus();
    void SetCellVis(int x, int y);

    inline size_t GetHitCellCount() const { return HitCells.size(); }
    inline const size_t* GetHitCellCountStart() const { return &HitCells.front(); }

protected:
    size_t Width = 0;
    size_t Height = 0;
    std::vector<int8_t> Cells;
    std::vector<int8_t> CellStatus;

    std::vector<size_t> HitCells;
};