#include "views/editor_view.h"

#include "raymath.h"
#include "rlgl.h"

constexpr float CellRenderSize = 32;

EditorView::EditorView(MapEditor& editor)
	: Editor(editor)
{
	ViewCamea.zoom = 1;
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

	float renderSize = 32;

	const auto& currentState = Editor.GetWorkingMap();

	for (int y = 0; y < currentState.GetHeight(); y++)
	{
		for (int x = 0; x < currentState.GetWidth(); x++)
		{
			if (currentState.GetCellSolid(x, y))
				DrawRectangleRec(Rectangle{ x * renderSize, y * renderSize, renderSize, renderSize }, WHITE);
			DrawRectangleLinesEx(Rectangle{ x * renderSize, y * renderSize, renderSize, renderSize }, 1, BLACK);
		}
	}

	// draw view location
	const auto& loc = Editor.GetViewLocation();
	Vector2 mapLoc = Vector2Scale(loc.Position, renderSize);

	DrawCircleV(mapLoc, renderSize / 3.0f, ColorAlpha(BLUE, 0.5f));
	DrawLineEx(mapLoc, Vector2Add(mapLoc, Vector2Scale(loc.Facing, renderSize * 2)), 2, ColorAlpha(SKYBLUE, 0.75f));

	DrawLineEx(Vector2{ -1, -1 }, Vector2{ renderSize, -1 }, 2, RED);
	DrawLineEx(Vector2{ -1, -1 }, Vector2{ - 1, renderSize }, 2, GREEN);

	DrawRectangleRec(Rectangle{ HoveredCell.x * renderSize, HoveredCell.y * renderSize, renderSize, renderSize }, ColorAlpha(YELLOW, 0.25f));

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
