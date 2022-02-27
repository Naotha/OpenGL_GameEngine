#include "EditorWidget.h"
#include <imgui/imgui.h>

void EditorWidget::Begin()
{
    ImGui::Begin(_title.c_str());
    _size = ImGui::GetContentRegionAvail();
}

void EditorWidget::End()
{
    ImGui::End();
}
