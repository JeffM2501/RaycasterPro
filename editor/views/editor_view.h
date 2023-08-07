#pragma once

#include "map_editor.h"

#include "raylib.h"

class EditorView
{
public:
	EditorView(MapEditor& editor);

	void Shutdown();

	void Show();

	void Pan(const Vector2& offset);
	void Zoom(float wheel);

	Vector2i GetHoverCell() const { return HoveredCell; }

	bool HasFocus = false;

	const Texture& GetTileTexture() const { return MapTilesTexture; }

	inline int GetTileCount() const
	{
		if (MapTilesTexture.height == 0)
			return 0; 
		
		return MapTilesTexture.width / MapTilesTexture.height;
	}

	inline Rectangle GetTileSourceRect(int tile)
	{
		return Rectangle{ float((tile-1) * MapTilesTexture.height), 0, float(MapTilesTexture.height), float(MapTilesTexture.height) };
	}
	

protected:
	MapEditor& Editor;

	Camera2D ViewCamea = { 0 };

	RenderTexture MapCacheTexture = { 0 };
	Texture MapTilesTexture = { 0 };

	Vector2i HoveredCell;

	void CheckCacheSize(int width, int height);
};