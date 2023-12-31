#pragma once

#include "command.h"

class EditorCommand : public Command
{
public:
    EditorCommand();
};

class QuitCommand : public EditorCommand
{
public:
    QuitCommand();
    void Process() override;
};

class NewMapCommand : public EditorCommand
{
public:
    NewMapCommand();
    void Process() override;
};

class OpenMapCommand : public EditorCommand
{
public:
    OpenMapCommand();
    void Process() override;
};

class SaveMapCommand : public EditorCommand
{
public:
    SaveMapCommand();
    void Process() override;
    bool IsEnabled() const override;
};

class SaveMapAsCommand : public EditorCommand
{
public:
    SaveMapAsCommand();
    void Process() override;
};

class UndoCommand : public EditorCommand
{
public:
    UndoCommand();
	void Process() override;
	bool IsEnabled() const override;
};

class RedoCommand : public EditorCommand
{
public:
    RedoCommand();
	void Process() override;
	bool IsEnabled() const override;
};

class ResizeCommand : public EditorCommand
{
public:
    ResizeCommand();
    void Process() override;
};

class BorderCommand : public EditorCommand
{
public:
    BorderCommand();
    void Process() override;
};

namespace EditorCommands
{
    extern QuitCommand Quit;
    extern NewMapCommand NewMap;
    extern OpenMapCommand OpenMap;
    extern SaveMapCommand SaveMap;
    extern SaveMapAsCommand SaveMapAs;

    extern UndoCommand Undo;
    extern RedoCommand Redo;

    extern ResizeCommand Resize;
    extern BorderCommand Border;

    CommandSet& GetCommandSet();
}