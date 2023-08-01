#include "map_editor.h"

MapEditor::MapEditor()
{
	Clear();
}

void MapEditor::Clear()
{
	Map defaultMap;
	FromMap(defaultMap);

	EditHistoryIndex = 0;
	EditHistory.clear();
}

void MapEditor::Load(std::string_view filepath)
{
	MapFilepath = filepath;

	EditHistoryIndex = 0;
	EditHistory.clear();

	MapSerializer serializer;

	FromMap(serializer.ReadResource(filepath));

	DirtyFlag = false;
}

void MapEditor::Save()
{
	MapSerializer serializer;
	serializer.WriteResource(ToMap(), MapFilepath);

	DirtyFlag = false;
}

void MapEditor::SetCell(const Vector2i& loction, uint8_t cellType)
{
	if (loction.x < 0 || loction.x >= CurrentState.Size.x || loction.y < 0 || loction.y > CurrentState.Size.y)
		return;

	SaveState("Set Cell");
	CurrentState.Cells[CurrentState.GetCellIndex(loction)] = cellType;
}

void MapEditor::Resize(int newX, int newY)
{
	SaveState("Resize");
	CurrentState.Cells.resize(newX * newY);

	const HistoryState& oldState = EditHistory[EditHistoryIndex - 1];

	for (int y = 0; y < newY; y++)
	{
		for (int x = 0; x < newX; x++)
		{
			uint8_t newCell = 0;

			if (x <= oldState.Size.x && y <= oldState.Size.y)
				newCell = oldState.Cells[oldState.GetCellIndex(x, y)];
			CurrentState.Cells[CurrentState.GetCellIndex(x, y)] = newCell;
		}
	}
}

void MapEditor::Undo()
{
	if (EditHistory.empty() || EditHistoryIndex == 0)
		return;

	SetDirty();
	EditHistoryIndex--;
	CurrentState = EditHistory[EditHistoryIndex];
}

void MapEditor::Redo()
{
	if (EditHistory.empty() || EditHistoryIndex >= EditHistory.size())
		return;
	SetDirty();
	EditHistoryIndex++;
	CurrentState = EditHistory[EditHistoryIndex];
}

bool MapEditor::CanUndo() const
{
	return !EditHistory.empty() && EditHistoryIndex > 0;
}

bool MapEditor::CanRedo() const
{
	return EditHistory.size() > EditHistoryIndex;
}

void MapEditor::FromMap(Map& map)
{
	CurrentState.Size.x = map.GetWidth();
	CurrentState.Size.y = map.GetHeight();

	CurrentState.Cells = map.GetCells();
}

Map MapEditor::ToMap()
{
	return Map(CurrentState.Cells, size_t(CurrentState.Size.x), size_t(CurrentState.Size.y));
}

void MapEditor::SaveState(std::string_view eventName)
{
	DirtyFlag = false;

	HistoryState state = CurrentState;
	state.EventName = eventName;

	if (EditHistory.size() > EditHistoryIndex)
		EditHistory.resize(EditHistoryIndex);

	EditHistory.emplace_back(std::move(state));
	EditHistoryIndex++;
}
