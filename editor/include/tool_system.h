#pragma once

#include <vector>

#include "imgui.h"

class Tool
{
public:
    virtual ~Tool() = default;

protected:
    friend class ToolSystem;

    Tool() = default;

    virtual void Show(bool isActve) = 0;
    virtual void OnActivate() {}
    virtual void OnDeactivate() {}

    virtual bool OnUpdate() { return false; }
};

class ToolSystem
{
public:

    template<class T>
    inline T* AddTool()
    {
        T* tool = new T();
        Tools.push_back(tool);
        return tool;
    }

    std::vector<Tool*> Tools;

    void Update();
};