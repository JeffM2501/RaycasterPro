#include "panels/material_picker_panel.h"

MaterialPickerPanel::MaterialPickerPanel()
{
	Name = "Materials";
	HorizontalAlignment = Panel::Alignment::Maxium;
	VerticalAlignment = Panel::Alignment::Minium;

	Size = ImVec2(200, 600);
}

void MaterialPickerPanel::OnShow()
{

}
