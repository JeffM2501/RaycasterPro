#include "tool_system.h"

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

	if (isAcitve)
	{
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImGui::GetForegroundDrawList()->AddRect(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_NavHighlight]));
	}
}
