#include "command.h"


//-----------------------Command----------------------------------//

Command::Command()
{
    GUID = rand();
}

void Command::Menu()
{
    CheckName();
    if (ImGui::MenuItem(ImGuiName.c_str(), ImGuiShortcut.c_str(), nullptr, IsEnabled()))
        Process();
}

void Command::Button()
{
    CheckName();
    ImGui::BeginDisabled(!IsEnabled());
    if (ImGui::Button(ImGuiName.c_str()))
    {
        Process();
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("%s %s", Name.c_str(), ImGuiShortcut.c_str());

    ImGui::EndDisabled();
}

bool CheckModKey(ImGuiKey desiredKey, std::set<ImGuiKey>& keyset)
{
    return ImGui::IsKeyDown(desiredKey) == (keyset.find(desiredKey) != keyset.end());
}

bool CheckModifyers(std::set<ImGuiKey>& keyset)
{
    if (!CheckModKey(ImGuiKey_ModCtrl, keyset))
        return false;

	if (!CheckModKey(ImGuiKey_ModShift, keyset))
		return false;

    if (!CheckModKey(ImGuiKey_ModAlt, keyset))
        return false;

    if (!CheckModKey(ImGuiKey_ModSuper, keyset))
        return false;

    return true;
}

void Command::CheckShortcut()
{
    if (CheckModifyers(Modifyers) && ImGui::IsKeyPressed(ShortcutKey) && IsEnabled())
        Process();
}

const char* GetKeyModifyerName(ImGuiKey modKey)
{
    switch (modKey)
    {
    case ImGuiKey_ModCtrl:
        return "Ctrl";
    case ImGuiKey_ModShift:
        return "Shift";
    case ImGuiKey_ModAlt:
        return "Alt";
    case ImGuiKey_ModSuper:
#ifdef _WIN32
        return "Windows";
#else
        return "Super"; 
#endif
    default:
        return "";
    }
}

void Command::CheckName()
{
    if (!ImGuiName.empty())
        return;

    ImGuiName = Icon;
    if (!Icon.empty())
        ImGuiName += " ";
    ImGuiName += Name;

    char temp[32] = { 0 };
    sprintf(temp, "###%d", GUID);
    ImGuiName += temp;

    ImGuiShortcut.clear();
    if (ShortcutKey != ImGuiKey_None)
    {
        for (const auto& modifyer : Modifyers)
        {
            if (!ImGuiShortcut.empty())
                ImGuiShortcut += "+";

            ImGuiShortcut += GetKeyModifyerName(modifyer);
        }
        if (!ImGuiShortcut.empty())
            ImGuiShortcut += "+";
        ImGuiShortcut += ImGui::GetKeyName(ShortcutKey);
    }
}

//-----------------------CommandSet----------------------------------//
void CommandSet::Add(Command& command)
{
	for (const Command* cmd : Commands)
	{
		if (command.GetGuid() == cmd->GetGuid())
			return;
	}

    Commands.push_back(&command);
}

void CommandSet::CheckShortcuts()
{
    for (Command* command : Commands)
        command->CheckShortcut();
}
