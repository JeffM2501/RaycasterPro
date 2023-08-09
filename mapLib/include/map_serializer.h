#pragma once

#include "map.h"

#include <string_view>

class MapSerializer
{
public:
    bool WriteResource(Map& map, std::string_view filepath);
    Map ReadResource(std::string_view filepath);
};