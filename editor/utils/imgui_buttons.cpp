#include "imgui_buttons.h"

#include "imgui.h"


namespace ImGui
{
    bool ConditionalButton(std::string_view id, bool enabled, const ImVec2& size)
    {
        bool returnValue = false;

        ImGui:BeginDisabled(enabled == 0);
        returnValue = ImGui::Button(id.data(), size);

        ImGui::EndDisabled();

        return returnValue;
    }
}