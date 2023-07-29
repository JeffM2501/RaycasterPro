#pragma once


#include <stdint.h>
#include <vector>

class Map
{
public:
    Map();

    uint8_t GetCell(int x, int y) const;
    bool IsCellVis(int x, int y) const;

    inline size_t GetWidth() const { return Width; }
    inline size_t GetHeight() const { return Height; }

    inline int GetCellIndex(int x, int y) const { return y * int(Width) + x; }

    inline void GetCellXY(size_t index, int& x, int& y) const
    { 
        y = int(index / Width);
        x = int(index % Width);
    }

    void SetCellVis(int x, int y);

    void StartFrame();

    inline const std::vector<size_t>& GetHitCellCountList() const { return HitCells; }

protected:
    size_t Width = 0;
    size_t Height = 0;
    std::vector<uint8_t> Cells;
    std::vector<uint8_t> CellStatus;

    std::vector<size_t> HitCells;

    uint8_t FrameVisId = 1;
};