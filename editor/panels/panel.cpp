#include "panels/panel.h"

#include "raylib.h"

void Panel::Show()
{
	if (!IsVisible())
		return;

	ImGui::SetNextWindowSize(Size);

	ImGuiWindowFlags flags = AdditionalFlags | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

	if (HorizontalAlignment != Alignment::Free && VerticalAlignment != Alignment::Free)
	{
		ImVec2 pos(0, ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y);
		if (HorizontalAlignment == Alignment::Maxium)
			pos.x = GetScreenWidth() - Size.x;

		if (VerticalAlignment == Alignment::Maxium)
			pos.y = GetScreenHeight() - Size.y;

		flags |= ImGuiWindowFlags_NoMove;

		ImGui::SetNextWindowPos(pos);
	}

	if (ImGui::Begin(Name.c_str(), &Visible, flags))
	{
		OnShow();
	}
	ImGui::End();


}
