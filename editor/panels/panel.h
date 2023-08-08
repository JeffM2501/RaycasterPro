#pragma once

#include "imgui.h"
#include <string>

class Panel
{
public:
	virtual ~Panel() = default;

	void Show();

	bool IsVisible() const { return Visible; }
	void SetVisible(bool vis = true) { Visible = vis; }

	inline const char* GetName() const { return Name.c_str(); }

protected:
	virtual void OnShow() = 0;

	std::string Name = "Panel";

	enum class Alignment
	{
		Minium,
		Maxium,
		Free
	};

	Alignment HorizontalAlignment = Alignment::Minium;
	Alignment VerticalAlignment = Alignment::Minium;

	ImVec2 Size = { 300, 300 };
	ImVec2 Offset = { 0, 0 };

	bool Visible = true;

	ImGuiWindowFlags AdditionalFlags = 0;
};