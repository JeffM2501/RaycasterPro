#pragma once

#include <string>
#include <vector>

#include "imgui.h"
#include "extras/IconsFontAwesome6.h"

class Tool
{
public:
    virtual ~Tool() = default;

protected:
    friend class ToolSystem;

    Tool() = default;

    inline virtual void Show(bool isActve) = 0;
    inline virtual void OnActivate() {}
    inline virtual void OnDeactivate() {}

    inline virtual bool OnUpdate() { return false; }

    inline virtual void OnClick() {}
};

class ButtonTool : public Tool
{
protected:
	virtual void Show(bool isActve) = 0;

    inline virtual bool OnUpdate() { return false; }

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
