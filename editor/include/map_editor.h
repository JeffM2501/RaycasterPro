#pragma once

#include "map.h"
#include "map_serializer.h"
#include "tool_system.h"

#include <string>
#include <string_view>

class HistoryState
{
public:
    std::string EventName;

	Vector2i Size;
	std::vector<uint8_t> Cells;

    inline int GetCellIndex(const Vector2i& location) const { return location.y * int(Size.x) + location.x; }
    inline int GetCellIndex(const int x, const int y) const { return y * int(Size.x) + x; }

    inline uint8_t GetCell(const int x, const int y) const 
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
};