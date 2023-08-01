#pragma once

#include "panels/panel.h"

class MaterialPickerPanel : public Panel
{
public:
	MaterialPickerPanel();

protected:
	void OnShow() override;
};