#pragma once

#include "map.h"
#include "map_serializer.h"
#include "tool_system.h"
#include "entity_location.h"

#include <string>
#include <string_view>

enum class CellState : uint8_t
{
    Empty = 0,
    Solid = 1,
};

class HistoryState
{
public:
    std::string EventName;

	Vector2i Size;
	std::vector<uint8_t> Cells;

    inline int GetCellIndex(const Vector2i& location) const { return location.y * int(Size.x) + location.x; }
    inline int GetCellIndex(const int x, const int y) const { return y * int(Size.x) + x; }

    inline bool GetCellSolid(const int x, const int y) const
    {
        if (x < 0 || x >= Size.x || y < 0 || y >= Size.y)
            return false;

        return Cells[y * int(Size.x) + x] != 0;
    }

    inline void SetCellState(const int x, const int y, CellState state, int tile = 1)
    {
        if (x < 0 || x >= Size.x || y < 0 || y >= Size.y)
            return;

        int index = y * int(Size.x) + x;

        switch (state)
        {
        case CellState::Empty:
            Cells[index] = 0;
            break;
        case CellState::Solid:
            Cells[index] = tile;
            break;
        default:
            break;
        }
    }

    inline uint8_t GetCellTile(const int x, const int y) const
    {
        if (x < 0 || x >= Size.x || y < 0 || y >= Size.y)
            return 0;

        return Cells[y * int(Size.x) + x];
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

    void SetCell(const Vector2i& loction, uint8_t cellType);

    std::string MapFilepath;
    bool Loaded = false;
  
    HistoryState& GetCurrentState();

    const std::vector<HistoryState>& GetEditHistory() const { return EditHistory; }
    const size_t GetCurrentEditHistoryIndex() const { return EditHistoryIndex; }

    inline const int GetCurrentMaterial() const { return CurrentMaterial; }
    inline void SetCurrentMaterial(int materialIndex) { CurrentMaterial = materialIndex; }

    inline ToolSystem& GetTools() { return ToolManager; }

    inline const Map& GetWorkingMap() 
    { 
        if (!MapValid)
            ToMap();
        MapValid = true;
        return WorkingMap;
    }

    inline EntityLocation& GetViewLocation() { return ViewLocaion; }

protected:
    void FromMap();
    void ToMap();

    void SaveState(std::string_view eventName);

    inline void SetDirty() { DirtyFlag = true; }

    bool DirtyFlag = false;
    size_t EditHistoryIndex = 0;
    std::vector<HistoryState> EditHistory;

    int CurrentMaterial = 1;

    ToolSystem ToolManager;

    Map WorkingMap;
    bool MapValid = false;

    EntityLocation ViewLocaion;
};