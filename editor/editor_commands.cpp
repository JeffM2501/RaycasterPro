#include "editor_commands.h"

#include "editor.h"
#include "map_editor.h"

#include "tinyfiledialogs.h"

constexpr int MapFilterPatternSize = 1;
constexpr  char* MapFilterPatterns[MapFilterPatternSize] = { "*.mres" };

namespace EditorCommands
{
	CommandSet GlobalCommandSet;
	CommandSet& GetCommandSet()
	{
		return GlobalCommandSet;
	}

    QuitCommand Quit;
    NewMapCommand NewMap;
    OpenMapCommand OpenMap;
    SaveMapCommand SaveMap;
    SaveMapAsCommand SaveMapAs;
	UndoCommand Undo;
	RedoCommand Redo;
}

EditorCommand::EditorCommand()
{
    EditorCommands::GetCommandSet().Add(*this);
}

QuitCommand::QuitCommand()
{
    Name = "Quit";
    ShortcutKey = ImGuiKey_F4;
    Modifyers.insert(ImGuiKey_ModAlt);
}

void QuitCommand::Process()
{
    Editor::Quit();
}

NewMapCommand::NewMapCommand()
{
    Name = "New";
    Icon = ICON_FA_SQUARE_PLUS;
    ShortcutKey = ImGuiKey_N;
    Modifyers.insert(ImGuiKey_ModCtrl);
}

void NewMapCommand::Process()
{
    if (Editor::GetActiveEditor().IsDirty())
    { 
        // give them a chance to save
        if (tinyfd_messageBox("Save existing map?", "The current map has unsaved changes, do you wish to save them?", "yesno", "question", 1) == 1)
            EditorCommands::SaveMap.Process();
    }
    Editor::GetActiveEditor().Clear();
}

OpenMapCommand::OpenMapCommand()
{
    Name = "Open...";
    Icon = ICON_FA_FOLDER_OPEN;
    ShortcutKey = ImGuiKey_O;
    Modifyers.insert(ImGuiKey_ModCtrl);
}

void OpenMapCommand::Process()
{
    const char* openFileName = tinyfd_openFileDialog("Open Map", Editor::GetActiveEditor().MapFilepath.c_str(), MapFilterPatternSize, MapFilterPatterns, nullptr, 0);
    if (openFileName)
        Editor::GetActiveEditor().Load(openFileName);
}

SaveMapCommand::SaveMapCommand()
{
    Name = "Save...";
    Icon = ICON_FA_FLOPPY_DISK;
    ShortcutKey = ImGuiKey_O;
    Modifyers.insert(ImGuiKey_ModCtrl);
}

void SaveMapCommand::Process()
{
    if (Editor::GetActiveEditor().MapFilepath.empty())
    {
        const char* saveFileName = tinyfd_saveFileDialog("Save Map As", Editor::GetActiveEditor().MapFilepath.c_str(), MapFilterPatternSize, MapFilterPatterns, nullptr);
        if (saveFileName != nullptr)
        {
            Editor::GetActiveEditor().MapFilepath = saveFileName;
        }
    }
    Editor::GetActiveEditor().Save();
}

bool SaveMapCommand::IsEnabled() const
{
    return Editor::GetActiveEditor().IsDirty();
}

SaveMapAsCommand::SaveMapAsCommand()
{
    Name = "Save As...";
    Icon = ICON_FA_FILE_EXPORT;
    ShortcutKey = ImGuiKey_O;
    Modifyers.insert(ImGuiKey_ModCtrl);
}

void SaveMapAsCommand::Process()
{
    const char* saveFileName = tinyfd_saveFileDialog("Save Map As", Editor::GetActiveEditor().MapFilepath.c_str(), MapFilterPatternSize, MapFilterPatterns, nullptr);
}

UndoCommand::UndoCommand()
{
	Name = "Undo";
	Icon = ICON_FA_CIRCLE_ARROW_LEFT;
	ShortcutKey = ImGuiKey_Z;
	Modifyers.insert(ImGuiKey_ModCtrl);
}

void UndoCommand::Process()
{
    Editor::GetActiveEditor().Undo();
}

bool UndoCommand::IsEnabled() const
{
    return Editor::GetActiveEditor().CanUndo();
}

RedoCommand::RedoCommand()
{
	Name = "Redo";
	Icon = ICON_FA_CIRCLE_ARROW_RIGHT;
	ShortcutKey = ImGuiKey_Y;
	Modifyers.insert(ImGuiKey_ModCtrl);
}

void RedoCommand::Process()
{
    Editor::GetActiveEditor().Redo();
}

bool RedoCommand::IsEnabled() const
{
    return Editor::GetActiveEditor().CanRedo();
}
