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

void Map::SetCellState(int x, int y, CellState state)
{
	if (x < 0 || x >= Width || y < 0 || y >= Height)
		return;

	int index = y * (int)Width + x;
	if (index < 0 || index >= Cells.size())
		return;

	Cells[index].State = state;
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

uint8_t Map::GetCellFlags(int x, int y) const
{
	if (x < 0 || x >= Width || y < 0 || y >= Height)
		return 0;
	return Cells[y * (int)Width + x].Flags;
}

void Map::SetCellFlags(int x, int y, uint8_t flag)
{
	if (x < 0 || x >= Width || y < 0 || y >= Height)
		return;

	int index = y * (int)Width + x;
	if (index < 0 || index >= Cells.size())
		return;

	Cells[index].Flags = flag;
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
