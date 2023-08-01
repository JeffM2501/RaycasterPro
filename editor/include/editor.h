#pragma once

#include "map_editor.h"
#include "views/editor_view.h"

namespace Editor
{
    void Quit();

    MapEditor& GetActiveEditor();
    EditorView& GetActiveView();
}