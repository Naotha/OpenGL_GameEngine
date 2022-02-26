#ifndef OPENGL_GAMEENGINE_EDITORGUI_H
#define OPENGL_GAMEENGINE_EDITORGUI_H

#include <vector>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include "Gui/EditorWindow.h"
#include "Window/Window.h"
#include "Engine/FBO.hpp"

class EditorGui {
public:
    EditorGui(Window& window);

    void Begin();
    void BeginDockSpace();
    void Render();
    void End();
    void Shutdown();

    void AddWindow(EditorWindow& widget);

private:
    void Init(Window& window);
    void InitStyle();

    std::vector<EditorWindow> _windows;
};


#endif //OPENGL_GAMEENGINE_EDITORGUI_H
