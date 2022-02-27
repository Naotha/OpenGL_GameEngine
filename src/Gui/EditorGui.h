#ifndef OPENGL_GAMEENGINE_EDITORGUI_H
#define OPENGL_GAMEENGINE_EDITORGUI_H

#include <vector>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include "Gui/EditorWidget.h"
#include "Window/Window.h"
#include "Engine/FBO.hpp"

class EditorGui {
public:
    EditorGui(Window& window);
    EditorGui(Window& window, std::vector<EditorWidget*> editorWidgets);

    void Begin();
    void Render();
    void End();
    void Shutdown();

    void AddWidget(EditorWidget* widget);

private:
    void Init(Window& window);
    void InitDockSpace();
    void InitStyle();

    std::vector<EditorWidget*> _editorWidgets;
};


#endif //OPENGL_GAMEENGINE_EDITORGUI_H
