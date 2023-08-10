#pragma once

#include "map.h"
#include "map_serializer.h"
#include "tool_system.h"
#include "entity_location.h"

#include <string>
#include <string_view>

class HistoryState
{
public:
    std::string EventName;

    Map Cells;

    int ToolId = -1;

    inline int GetWidth() const { return Cells.GetWidth(); }
    inline int GetHeight() const { return Cells.GetHeight(); }

	inline void SetCellState(const int x, const int y, CellState state, int tile = 1)
	{
		switch (state)
		{
			case CellState::Empty:
				Cells.SetCellSolid(x,y, false);
				break;
			case CellState::Solid:
                Cells.SetCellSolid(x, y, true);
                Cells.SetCellTile(x, y, tile);
				break;
			default:
				break;
		}
	}
};

class MapEditor
{
public:
    MapEditor();

    void Update();

    void Clear();
    void Load(std::string_view filepath);
    void Save();

    inline bool IsDirty() const { return DirtyFlag; }

    void Undo();
    void Redo();
    bool CanUndo() const;
    bool CanRedo() const;

    void SetHistoryIndex(int index);

    void Resize(int newX, int newY);

    void SetCell(int x, int y, CellState cellState, uint8_t cellTile, int toolId = -1);
    inline void SetCell(const Vector2i& loction, CellState cellState, uint8_t cellType, int toolId = -1)
    {
        SetCell(loction.x, loction.y, cellState, cellType, toolId);
    }
    inline void SetCell(const Vector2& loction, CellState cellState, uint8_t cellType, int toolId = -1)
    {
        SetCell(Vector2i{ int(floorf(loction.x)),int(floorf(loction.y)) }, cellState, cellType, toolId);
    }

    std::string MapFilepath;
    bool Loaded = false;
  
    HistoryState& GetCurrentState();

    const std::vector<HistoryState>& GetEditHistory() const { return EditHistory; }
    const size_t GetCurrentEditHistoryIndex() const { return EditHistoryIndex; }

    inline const int GetCurrentMaterial() const { return CurrentMaterial; }
    inline void SetCurrentMaterial(int materialIndex) { CurrentMaterial = materialIndex; }

    inline ToolSystem& GetTools() { return ToolManager; }

    inline Map& GetWorkingMap() 
    { 
        return EditHistory[EditHistoryIndex].Cells;
    }

    inline EntityLocation& GetViewLocation() { return ViewLocaion; }

protected:
    void SaveState(std::string_view eventName, int toolId = -1);

    inline void SetDirty() { DirtyFlag = true; }

    bool DirtyFlag = false;
    size_t EditHistoryIndex = 0;
    std::vector<HistoryState> EditHistory;

    int CurrentMaterial = 1;

    ToolSystem ToolManager;

    EntityLocation ViewLocaion;
};