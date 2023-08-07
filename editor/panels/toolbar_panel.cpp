#include "panels/toolbar_panel.h"

#include "extras/IconsFontAwesome6.h"
#include "editor.h"
#include "tool_system.h"

ToolbarPanel::ToolbarPanel()
{
	Name = "Tools";
	AdditionalFlags |= ImGuiWindowFlags_NoDecoration;

	HorizontalAlignment = Panel::Alignment::Minium;
	VerticalAlignment = Panel::Alignment::Minium;

	Size = ImVec2(26 *6, 32);
}

void ToolbarPanel::OnShow()
{
	auto& toolManager = Editor::GetActiveEditor().GetTools();

	Size = ImVec2(26.0f * toolManager.GetTools().size(), 32);
	
	for (auto* tool : toolManager.GetTools())
	{
		tool->Show(tool == toolManager.GetActiveTool());
		ImGui::SameLine();
	}
}
