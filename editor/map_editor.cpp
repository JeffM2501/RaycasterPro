#include "map_editor.h"

MapEditor::MapEditor()
{
	Clear();
}

void MapEditor::Update()
{
	ToolManager.Update();
}

void MapEditor::Clear()
{
	ViewLocaion.Position = Vector2{ 5, 5 };
	ViewLocaion.Facing.x = 1;

	EditHistoryIndex = 0;
	EditHistory.clear();
	EditHistory.emplace_back(HistoryState{ "New Map" });
}

void MapEditor::Load(std::string_view filepath)
{
	MapFilepath = filepath;

	EditHistoryIndex = 0;
	EditHistory.clear();
	EditHistory.emplace_back(HistoryState{ "Load Map" });

	MapSerializer serializer;
	EditHistory.back().Cells = serializer.ReadResource(filepath);

	DirtyFlag = false;
}

void MapEditor::Save()
{
	MapSerializer serializer;
	serializer.WriteResource(GetWorkingMap(), MapFilepath);

	DirtyFlag = false;
}

HistoryState& MapEditor::GetCurrentState()
{
	return EditHistory[EditHistoryIndex];
}

void MapEditor::SetCell(const Vector2i& location, uint8_t cellType, int toolId)
{
	if (location.x < 0 || location.x >= GetCurrentState().Cells.GetWidth() || location.y < 0 || location.y > GetCurrentState().Cells.GetHeight())
		return;

	SaveState("Set Cell", toolId);
	GetCurrentState().SetCellState(location.x, location.y, cellType == 0 ? CellState::Empty : CellState::Solid, cellType);
}

void MapEditor::Resize(int newX, int newY)
{
	SaveState("Resize");
	auto& state = GetCurrentState();
	state.Cells.Resize(newX,newY);

	const HistoryState& oldState = EditHistory[EditHistoryIndex - 1];

	for (int y = 0; y < newY; y++)
	{
		for (int x = 0; x < newX; x++)
		{
			uint8_t newCell = 0;

			if (x < oldState.GetWidth() && y < oldState.GetHeight())
				newCell = oldState.Cells.GetCellTile(x, y);
			GetCurrentState().SetCellState(x,y, newCell == 0 ? CellState::Empty : CellState::Solid, newCell);
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

void MapEditor::SetHistoryIndex(int index)
{
	if (EditHistory.empty() || index < 0 || index >= EditHistory.size())
		return;

	SetDirty();
	EditHistoryIndex = size_t(index);
}

bool MapEditor::CanUndo() const
{
	return !EditHistory.empty() && EditHistoryIndex > 0;
}

bool MapEditor::CanRedo() const
{
	return EditHistory.size() > EditHistoryIndex;
}

void MapEditor::SaveState(std::string_view eventName, int toolId)
{
	DirtyFlag = true;

	HistoryState& currentState = GetCurrentState();

	// can we merge this edit with the last one?
	if (toolId != -1 && currentState.ToolId != -1 && currentState.ToolId == toolId)
		return;

	// copy our current state
	HistoryState state = currentState;
	state.EventName = eventName;
	state.ToolId = toolId;

	// trim off everything after now
	if (EditHistory.size() > EditHistoryIndex+1)
		EditHistory.resize(EditHistoryIndex+1);

	// add the new state to the end
	EditHistory.emplace_back(std::move(state));
	EditHistoryIndex++;
}
