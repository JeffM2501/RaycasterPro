#include "panels/material_picker_panel.h"

#include "editor.h"
#include "rlImGui.h"

MaterialPickerPanel::MaterialPickerPanel()
{
	Name = "Materials";
	HorizontalAlignment = Panel::Alignment::Maxium;
	VerticalAlignment = Panel::Alignment::Minium;

	Size = ImVec2(200, 600);
}

void MaterialPickerPanel::OnShow()
{
	if (ImGui::BeginChild("MaterialList"))
	{
		auto& view = Editor::GetActiveView();

		ImVec2 selectableSize(150, 100);
		ImVec2 buttonSize(100, 100);

		for (int i = 0; i < view.GetTileCount(); i++)
		{
			char text[32] = { 0 };
			sprintf(text, "###Mat%d", i);
			int cursorX = ImGui::GetCursorPosX();
			bool selected = ImGui::Selectable(text, i == Editor::GetActiveEditor().GetCurrentMaterial(), ImGuiSelectableFlags_AllowItemOverlap, selectableSize);

			if (selected)
				Editor::GetActiveEditor().SetCurrentMaterial(i);

			ImGui::SameLine();
			ImGui::SetCursorPosX(cursorX+1);
			rlImGuiImageRect(&view.GetTileTexture(), buttonSize.x, buttonSize.y, view.GetTileSourceRect(i));

			if (i == Editor::GetActiveEditor().GetCurrentMaterial())
			{
				ImGui::SameLine();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonSize.y * 0.5f);
				ImGui::TextUnformatted(ICON_FA_CIRCLE_CHEVRON_LEFT);
			}
		}

		ImGui::EndChild();
	}
}
