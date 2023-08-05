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

void ButtonTool::Show(bool isActve)
{
	ImGui::togglebu
}
