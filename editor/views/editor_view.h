#pragma once

#include "map_editor.h"

#include "raylib.h"

struct Rectangle;
struct Vector2;
struct RenderTexture;

class EditorView
{
public:
	EditorView(MapEditor& editor);

	void Shutdown();

	void Show();

	void Pan(const Vector2& offset);
	void Zoom(float wheel);

	Vector2i GetHoverCell() const { return HoveredCell; }

protected:
	MapEditor& Editor;

	Camera2D ViewCamea = { 0 };

	RenderTexture MapCacheTexture = { 0 };

	Vector2i HoveredCell;

	void CheckCacheSize(int width, int height);
};