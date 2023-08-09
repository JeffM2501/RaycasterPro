#include "map.h"

Map::Map()
{
    Width = 24;
    Height = 24; 
    Cells.resize(Width * Height);
}

bool Map::GetCellSolid(int x, int y) const
{
    if (x < 0 || x >= Width || y < 0 || y >= Height)
        return false;

    int index = y * (int)Width + x;
    if (index < 0 || index >= Cells.size())
        return false;

    return Cells[index].State != CellState::Empty;
}

void Map::SetCellSolid(int x, int y, bool solid)
{
	if (x < 0 || x >= Width || y < 0 || y >= Height)
		return;

	int index = y * (int)Width + x;
	if (index < 0 || index >= Cells.size())
		return;

	Cells[index].State = solid ? CellState::Solid : CellState::Empty;
}

uint8_t Map::GetCellTile(int x, int y) const
{
    if (x < 0 || x >= Width || y < 0 || y >= Height)
        return 0;
    return Cells[y * (int)Width + x].Tile;
}

void Map::SetCellTile(int x, int y, uint8_t tile)
{
	if (x < 0 || x >= Width || y < 0 || y >= Height)
		return;

	int index = y * (int)Width + x;
	if (index < 0 || index >= Cells.size())
		return;

    Cells[index].State = CellState::Solid;
	Cells[index].Tile = tile;
}

bool Map::GetCellPassable(int x, int y) const
{
    if (x < 0 || x >= Width || y < 0 || y >= Height)
        return false;

    int index = y * (int)Width + x;
    if (index < 0 || index >= Cells.size())
        return false;

    return Cells[index].State == CellState::Empty;
}

void Map::Resize(int newWidth, int newHeight)
{
	Cells.resize(newWidth * newHeight);
    Width = newWidth;
	Height = newHeight;
}
