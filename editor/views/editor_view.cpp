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

	HoveredCell.x = int(floorf(mouseWorldPos.x / CellRenderSize));
	HoveredCell.y = int(floorf(mouseWorldPos.y / CellRenderSize));

	CheckCacheSize(GetScreenWidth(), GetScreenHeight());

	BeginTextureMode(MapCacheTexture);
	ClearBackground(DARKGRAY);

	BeginMode2D(ViewCamea);

	float renderSize = 32;

	for (int y = 0; y < Editor.CurrentState.Size.y; y++)
	{
		for (int x = 0; x < Editor.CurrentState.Size.x; x++)
		{
			if (Editor.CurrentState.GetCell(x, y) != 0)
				DrawRectangle(x * renderSize, y * renderSize, renderSize, renderSize, WHITE);
			DrawRectangleLines(x * renderSize, y * renderSize, renderSize, renderSize, BLACK);
		}
	}

	DrawLine(-1, -1, renderSize, -1, RED);
	DrawLine(-1, -1, -1, renderSize, GREEN);

	DrawRectangle(HoveredCell.x * renderSize, HoveredCell.y * renderSize, renderSize, renderSize, ColorAlpha(YELLOW, 0.25f));

	EndMode2D();

	EndTextureMode();

	DrawTexture(MapCacheTexture.texture, 0, 0, WHITE);

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		Editor.SetCell(HoveredCell, 0);
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
	if (MapCacheTexture.id != 0 && MapCacheTexture.texture.width == width && MapCacheTexture.texture.height && height)
		return;

	if (MapCacheTexture.id != 0)
		UnloadRenderTexture(MapCacheTexture);

	MapCacheTexture = LoadRenderTexture(width, height);
}
