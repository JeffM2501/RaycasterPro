#pragma once

#include "panels/panel.h"

class ToolbarPanel : public Panel
{
public:
	ToolbarPanel();

protected:
	void OnShow() override;
};