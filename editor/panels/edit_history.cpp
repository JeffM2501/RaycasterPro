#include "panels/edit_history.h"

#include "extras/IconsFontAwesome6.h"
#include "editor.h"

EditHistoryPanel::EditHistoryPanel()
{
	Name = "Edit History";
	HorizontalAlignment = Panel::Alignment::Maxium;
	VerticalAlignment = Panel::Alignment::Maxium;

	Size = ImVec2(200, 200);
}

void EditHistoryPanel::OnShow()
{
	auto& activeEditor = Editor::GetActiveEditor();

	bool scrollToBottom = ItemCount != int(activeEditor.GetEditHistory().size());

	for (int i = 0; i < activeEditor.GetEditHistory().size(); i++)
	{
		const auto& item = activeEditor.GetEditHistory()[i];
		bool selected = i == activeEditor.GetCurrentEditHistoryIndex();
		char tempName[512] = { 0 };

		ImGui::BeginDisabled(i > activeEditor.GetCurrentEditHistoryIndex());
		sprintf(tempName, "%s %s", selected ? ICON_FA_CIRCLE_CHEVRON_RIGHT : " ", item.EventName.c_str());

		ImGui::Selectable(tempName, false, ImGuiSelectableFlags_AllowDoubleClick);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			activeEditor.SetHistoryIndex(i);
		ImGui::EndDisabled();
	}
	if (scrollToBottom)
		ImGui::SetScrollHereY();

	ItemCount = int(activeEditor.GetEditHistory().size());
}
