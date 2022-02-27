#include "EditorWindow.h"
#include <imgui/imgui.h>

void EditorWindow::Begin()
{
    ImGui::Begin(_title.c_str());
    _size = ImGui::GetContentRegionAvail();
}

void EditorWindow::End()
{
    ImGui::End();
}
