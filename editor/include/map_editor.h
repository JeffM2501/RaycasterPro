#pragma once

#include "map.h"
#include "map_serializer.h"

#include <string>
#include <string_view>

class MapEditor
{
public:
    void Clear();
    void Load(std::string_view filepath);
    void Save();

    void SetCell(const Vector2i& loction, uint8_t cellType);

    std::string MapFilepath;
    bool Loaded = false;
    bool Dirty = false;

    Map MapData;
};