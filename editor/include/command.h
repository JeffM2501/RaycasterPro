#pragma once

#include "imgui.h"
#include "extras/IconsFontAwesome6.h"

#include <string>
#include <set>
#include <vector>

class Command
{
public:
    Command();
    virtual ~Command() = default;

    // non copyable
    Command(const Command&) = delete;
    Command& operator= (const Command&) = delete;

    inline int GetGuid() const { return GUID; }

    void Menu();
    void Button();

    virtual void CheckShortcut();

protected:
    virtual bool IsEnabled() const { return true; }
    virtual void Process() = 0;

    std::string Name;
    std::string Icon;

    ImGuiKey ShortcutKey = ImGuiKey::ImGuiKey_None;
    std::set<ImGuiKey> Modifyers;

private:
    void CheckName();

    int GUID = 0;

    std::string ImGuiName;
    std::string ImGuiShortcut;
};

class CommandSet
{
public:
    void Add(Command& command);
    
    void CheckShortcuts();
protected:
    std::vector<Command*> Commands;
};