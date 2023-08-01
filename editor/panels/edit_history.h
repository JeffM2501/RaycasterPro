#pragma once

#include "panels/panel.h"

class EditHistoryPanel : public Panel
{
public:
	EditHistoryPanel();

protected:
	void OnShow() override;

	int ItemCount = 0;
};