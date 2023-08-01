#include "map_editor.h"

MapEditor::MapEditor()
{
	Clear();
}

void MapEditor::Clear()
{
	EditHistoryIndex = 0;
	EditHistory.clear();
	EditHistory.emplace_back(HistoryState{ "New Map" });

	Map defaultMap;
	FromMap(defaultMap);
}

void MapEditor::Load(std::string_view filepath)
{
	MapFilepath = filepath;

	EditHistoryIndex = 0;
	EditHistory.clear();
	EditHistory.emplace_back(HistoryState{ "Load Map" });

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

HistoryState& MapEditor::GetCurrentState()
{
	return EditHistory[EditHistoryIndex];
}

void MapEditor::SetCell(const Vector2i& loction, uint8_t cellType)
{
	if (loction.x < 0 || loction.x >= GetCurrentState().Size.x || loction.y < 0 || loction.y > GetCurrentState().Size.y)
		return;

	SaveState("Set Cell");
	GetCurrentState().Cells[GetCurrentState().GetCellIndex(loction)] = cellType;
}

void MapEditor::Resize(int newX, int newY)
{
	SaveState("Resize");
	GetCurrentState().Cells.resize(newX * newY);

	const HistoryState& oldState = EditHistory[EditHistoryIndex - 1];

	for (int y = 0; y < newY; y++)
	{
		for (int x = 0; x < newX; x++)
		{
			uint8_t newCell = 0;

			if (x <= oldState.Size.x && y <= oldState.Size.y)
				newCell = oldState.Cells[oldState.GetCellIndex(x, y)];
			GetCurrentState().Cells[GetCurrentState().GetCellIndex(x, y)] = newCell;
		}
	}
}

void MapEditor::Undo()
{
	if (EditHistory.empty() || EditHistoryIndex == 0)
		return;

	SetDirty();
	EditHistoryIndex--;
}

void MapEditor::Redo()
{
	if (EditHistory.empty() || EditHistoryIndex >= EditHistory.size()-1)
		return;
	SetDirty();
	EditHistoryIndex++;
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
	GetCurrentState().Size.x = map.GetWidth();
	GetCurrentState().Size.y = map.GetHeight();

	GetCurrentState().Cells = map.GetCells();
}

Map MapEditor::ToMap()
{
	return Map(GetCurrentState().Cells, size_t(GetCurrentState().Size.x), size_t(GetCurrentState().Size.y));
}

void MapEditor::SaveState(std::string_view eventName)
{
	DirtyFlag = false;

	// copy our current state
	HistoryState state = GetCurrentState();
	state.EventName = eventName;

	// trim off everything after now
	if (EditHistory.size() > EditHistoryIndex+1)
		EditHistory.resize(EditHistoryIndex+1);

	// add the new state to the end
	EditHistory.emplace_back(std::move(state));
	EditHistoryIndex++;
}
