#ifndef OPENGL_GAMEENGINE_EDITORWIDGET_H
#define OPENGL_GAMEENGINE_EDITORWIDGET_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <imgui/imgui.h>

class EditorWidget {
public:
    EditorWidget(std::string title) : _title(title) {}

    void Begin();
    void End();
    virtual void Render() {};

    std::string GetTitle() { return _title; }
    glm::vec2 GetSize() { return glm::vec2(_size.x, _size.y); }
protected:
    std::string _title;
    ImVec2 _size;
};


#endif //OPENGL_GAMEENGINE_EDITORWIDGET_H
