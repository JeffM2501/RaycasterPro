#pragma once

#include <string_view>

#include "imgui.h"

namespace ImGui
{
    bool ConditionalButton(std::string_view id, bool enabled, const ImVec2& size = ImVec2(0, 0));
}