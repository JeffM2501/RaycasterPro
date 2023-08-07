#include "tool_system.h"

#include "tools/editor_tools.h"

ToolSystem::ToolSystem()
{
	AddTool<SelectTool>();
	AddTool<EraserTool>();
	AddTool<PaintWallTool>();
	AddTool<PaintFloorTool>();
	AddTool<PaintCeilingTool>();
	AddTool<SetDoorTool>();
	AddTool<SetObjectTool>();
}

ToolSystem::~ToolSystem()
{
	for (auto* tool : Tools)
		delete(tool);
}

void ToolSystem::Update()
{
	for (auto* tool : Tools)
	{
		if (tool->OnUpdate())
		{
			if (ActiveTool)
				ActiveTool->OnDeactivate();

			ActiveTool = tool;
			ActiveTool->OnActivate();
		}
	}
}

void ButtonTool::Show(bool isAcitve)
{
	IsActivated = ImGui::Button(Icon.c_str());

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip(ToolTip.c_str());

	if (isAcitve)
	{
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_NavHighlight]));
	}
}
