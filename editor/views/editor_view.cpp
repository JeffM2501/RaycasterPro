#include "views/editor_view.h"

#include "raymath.h"
#include "rlgl.h"

constexpr float CellRenderSize = 32;

EditorView::EditorView(MapEditor& editor)
	: Editor(editor)
{
	ViewCamea.zoom = 1;

	ViewCamea.offset.x = GetScreenWidth() * 0.5f;
	ViewCamea.offset.y = GetScreenHeight() * 0.5f;

	ViewCamea.target.x = -editor.GetWorkingMap().GetWidth() / 2 * CellRenderSize;

}

void EditorView::Shutdown()
{
	if (MapCacheTexture.id != 0)
		UnloadRenderTexture(MapCacheTexture);

	MapCacheTexture.id = 0;
}

void EditorView::Show()
{
    Vector2 mousePos = GetMousePosition();
    mousePos.y = GetScreenHeight() - mousePos.y;
    Vector2 mouseWorldPos = GetScreenToWorld2D(mousePos, ViewCamea);

	Vector2 mouseMapPos = { mouseWorldPos.x / CellRenderSize, mouseWorldPos.y / CellRenderSize };

	if (HasFocus)
	{
		HoveredCell.x = int(floorf(mouseMapPos.x));
		HoveredCell.y = int(floorf(mouseMapPos.y));
	}

	CheckCacheSize(GetScreenWidth(), GetScreenHeight());

	BeginTextureMode(MapCacheTexture);
	ClearBackground(DARKGRAY);

	BeginMode2D(ViewCamea);

	

	const auto& currentState = Editor.GetWorkingMap();

	for (int y = 0; y < currentState.GetHeight(); y++)
	{
		for (int x = 0; x < currentState.GetWidth(); x++)
		{
			if (currentState.GetCellSolid(x, y))
				DrawRectangleRec(Rectangle{ x * CellRenderSize, y * CellRenderSize, CellRenderSize, CellRenderSize }, WHITE);
			DrawRectangleLinesEx(Rectangle{ x * CellRenderSize, y * CellRenderSize, CellRenderSize, CellRenderSize }, 1, BLACK);
		}
	}

	// draw view location
	const auto& loc = Editor.GetViewLocation();
	Vector2 mapLoc = Vector2Scale(loc.Position, CellRenderSize);

	DrawCircleV(mapLoc, CellRenderSize / 3.0f, ColorAlpha(BLUE, 0.5f));
	DrawLineEx(mapLoc, Vector2Add(mapLoc, Vector2Scale(loc.Facing, CellRenderSize * 2)), 2, ColorAlpha(SKYBLUE, 0.75f));

	DrawLineEx(Vector2{ -1, -1 }, Vector2{ CellRenderSize, -1 }, 2, RED);
	DrawLineEx(Vector2{ -1, -1 }, Vector2{ - 1, CellRenderSize }, 2, GREEN);

	DrawRectangleRec(Rectangle{ HoveredCell.x * CellRenderSize, HoveredCell.y * CellRenderSize, CellRenderSize, CellRenderSize }, ColorAlpha(YELLOW, 0.25f));

	EndMode2D();

	EndTextureMode();
	DrawTexture(MapCacheTexture.texture, 0, 0, WHITE);

	if (HasFocus)
	{ 
        auto* activeTool = Editor.GetTools().GetActiveTool();
		if (activeTool)
		{
			activeTool->OnHover(mouseMapPos);

			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
				activeTool->OnClick(mouseMapPos);
		}
	}
}

void EditorView::Pan(const Vector2& offset)
{
	Vector2 delta  = Vector2Scale(offset, -1.0f / ViewCamea.zoom);
	ViewCamea.target = Vector2Add(ViewCamea.target, Vector2{ delta.x, -delta.y });
}

void EditorView::Zoom(float wheel)
{
	if (wheel != 0)
	{
		Vector2 mousePos = GetMousePosition();
		mousePos.y = GetScreenHeight() - mousePos.y;

		// get the world point that is under the mouse
		Vector2 mouseWorldPos = GetScreenToWorld2D(mousePos, ViewCamea);

		// set the offset to where the mouse is
		ViewCamea.offset = mousePos;

		// set the target to match, so that the camera maps the world space point under the cursor to the screen space point under the cursor at any zoom
		ViewCamea.target = mouseWorldPos;

		// zoom
		ViewCamea.zoom += wheel * 0.125f;
		if (ViewCamea.zoom < 0.125f)
			ViewCamea.zoom = 0.125f;
	}
}

void EditorView::CheckCacheSize(int width, int height)
{
	if (MapTilesTexture.id == 0)
		MapTilesTexture = LoadTexture("resources/textures/textures.png");

	if (MapCacheTexture.id != 0 && MapCacheTexture.texture.width == width && MapCacheTexture.texture.height && height)
		return;

	if (MapCacheTexture.id != 0)
		UnloadRenderTexture(MapCacheTexture);

	MapCacheTexture = LoadRenderTexture(width, height);
}
