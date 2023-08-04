#include "panels/toolbar_panel.h"

#include "extras/IconsFontAwesome6.h"
#include "editor.h"

ToolbarPanel::ToolbarPanel()
{
	Name = "Tools";
	AdditionalFlags |= ImGuiWindowFlags_NoDecoration;

	HorizontalAlignment = Panel::Alignment::Minium;
	VerticalAlignment = Panel::Alignment::Minium;

	Size = ImVec2(32*6, 32);
}

void ToolbarPanel::OnShow()
{
	ImGui::Button(ICON_FA_ARROW_POINTER);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", "Select");
	ImGui::SameLine();

	ImGui::Button(ICON_FA_PAINT_ROLLER);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", "Paint Walls");
	ImGui::SameLine();

	ImGui::Button(ICON_FA_ARROW_DOWN_LONG);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", "Paint Floor");
	ImGui::SameLine();

	ImGui::Button(ICON_FA_ARROW_UP_LONG);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", "Paint Ceiling");
	ImGui::SameLine();

	ImGui::Button(ICON_FA_DOOR_OPEN);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", "Set Door");
	ImGui::SameLine();

	ImGui::Button(ICON_FA_PERSON);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", "Set Object");
}
