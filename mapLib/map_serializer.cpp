#include "map_serializer.h"

constexpr int CurrentMapVersion = 1;

bool MapSerializer::WriteResource(const Map& map, std::string_view filepath)
{
    FILE* fp = fopen(filepath.data(), "w");

    if (!fp)
        return false;

    bool valid = true;

    int version = CurrentMapVersion;
    fwrite(&version, 4, 1, fp);
  
    int xSize = int(map.GetWidth());
    int ySize = int(map.GetHeight());

    if (fwrite(&xSize, 4, 1, fp) != 4 || fwrite(&ySize, 4, 1, fp) != 4)
        valid = false;

    if (xSize == 0 || ySize == 0)
        valid = false;

    if (valid)
    {
        fwrite(&map.GetCells()[0], xSize*ySize, 1, fp);
    }

    fclose(fp);
    return valid;
}

Map MapSerializer::ReadResource(std::string_view filepath)
{
    FILE* fp = fopen(filepath.data(), "r");

    if (!fp)
        return Map();

    bool valid = true;

    int version = 0;
    fread(&version, 4, 1, fp);
    switch (version)
    {
    default:
        valid = false;
        break;

        // other versions here

    case CurrentMapVersion:
        break;
    }

    int xSize = 0;
    int ySize = 0;

    if (fread(&xSize, 4, 1, fp) != 4 || fread(&ySize, 4, 1, fp) != 4)
        valid = false;

    if (xSize == 0 || ySize == 0)
        valid = false;

    std::vector<uint8_t> cells;
    if (valid)
    {
        cells.resize(xSize * ySize);
        fread(&cells[0], xSize * ySize, 1, fp);
    }
    fclose(fp);

    if (!valid)
        return Map();

    return Map(cells, xSize, ySize);
}
