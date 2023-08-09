#include "map_serializer.h"

constexpr int CurrentMapVersion = 2;

bool MapSerializer::WriteResource(Map& map, std::string_view filepath)
{
    FILE* fp = fopen(filepath.data(), "w");

    if (!fp)
        return false;

    bool valid = true;

    int version = CurrentMapVersion;
    fwrite(&version, 4, 1, fp);
  
    int xSize = int(map.GetWidth());
    int ySize = int(map.GetHeight());

    if (fwrite(&xSize, 4, 1, fp) != 1 || fwrite(&ySize, 4, 1, fp) != 1)
        valid = false;

    if (xSize == 0 || ySize == 0)
        valid = false;

    if (valid)
    {
        for (auto& cell : map.GetCellsList())
        {
            fwrite(&cell.State, 1, 1, fp);
            fwrite(&cell.Tile, 1, 1, fp);
        }
    }

    fclose(fp);
    return valid;
}

Map MapSerializer::ReadResource(std::string_view filepath)
{
    Map map;

    FILE* fp = fopen(filepath.data(), "r");

    if (!fp)
        return map;

    bool valid = true;

    int version = 0;
    fread(&version, 4, 1, fp);
    switch (version)
    {
		default:
			valid = false;
			break;

        // other versions here

		case 1:
        {
			int xSize = 0;
			int ySize = 0;

			if (fread(&xSize, 4, 1, fp) != 1 || fread(&ySize, 4, 1, fp) != 1)
				valid = false;

			if (xSize == 0 || ySize == 0)
				valid = false;

			std::vector<uint8_t> cells;
			if (valid)
			{
				map.GetCellsList().resize(xSize * ySize);
				for (auto& cell : map.GetCellsList())
				{
					fread(&cell.Tile, 1, 1, fp);
                    cell.State = cell.Tile == 0 ? CellState::Empty : CellState::Solid;
				}
			}
        }
		break;

        case CurrentMapVersion:
        {
			int xSize = 0;
			int ySize = 0;

			if (fread(&xSize, 4, 1, fp) != 1 || fread(&ySize, 4, 1, fp) != 1)
				valid = false;

			if (xSize == 0 || ySize == 0)
				valid = false;

			std::vector<uint8_t> cells;
			if (valid)
			{
				map.GetCellsList().resize(xSize * ySize);
				for (auto& cell : map.GetCellsList())
				{
					fread(&cell.State, 1, 1, fp);
					fread(&cell.Tile, 1, 1, fp);
				}
			}
        }
        break;
    }
    
    fclose(fp);

    return map;
}
