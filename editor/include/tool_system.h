#pragma once

#include <string>
#include <vector>

#include "imgui.h"
#include "extras/IconsFontAwesome6.h"

class Tool
{
public:
    virtual ~Tool() = default;
    inline virtual void Show(bool isActve) = 0;
    inline virtual void OnClick(const ImVec2& position) {}
    inline virtual void OnHover(const ImVec2& position) {}

protected:
    friend class ToolSystem;

    Tool() = default;
    inline virtual void OnActivate() {}
    inline virtual void OnDeactivate() {}

    inline virtual bool OnUpdate() { return false; }
};

class ButtonTool : public Tool
{
public:
    void Show(bool isActve) override;

protected:
    inline virtual bool OnUpdate() { return IsActivated; }
    bool IsActivated = false;
    std::string Icon = ICON_FA_QUESTION;
    std::string ToolTip;
};

class ToolSystem
{
protected:
    std::vector<Tool*> Tools;
	Tool* ActiveTool = nullptr;

public:
    ToolSystem();
    virtual ~ToolSystem();

    template<class T>
    inline T* AddTool()
    {
        T* tool = new T();
        Tools.push_back(tool);
        return tool;
    }
    void Update();

    inline Tool* GetActiveTool() const { return ActiveTool; }

    inline const std::vector<Tool*>& GetTools() const { return Tools; }
};
