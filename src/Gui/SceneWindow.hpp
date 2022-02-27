#ifndef OPENGL_GAMEENGINE_SCENEWINDOW_HPP
#define OPENGL_GAMEENGINE_SCENEWINDOW_HPP

#include "Gui/EditorWindow.h"
#include "Engine/FBO.hpp"
#include <imgui/imgui.h>
#include <iostream>
#include <glm/glm.hpp>

class SceneWindow : public EditorWindow{
public:
    SceneWindow(FBO& sceneBuffer, glm::mat4& model, glm::mat4& view, glm::mat4& projection)
        : EditorWindow("Scene"), _sceneBuffer(sceneBuffer), _model(model), _view(view), _projection(projection)
        {
            _translate = false;
            _rotate = false;
            _scale = false;
        }

    virtual void Render() final
    {
        Begin();

        ResizeCheck();
        RenderFBO();
        UseImGuizmo();

        End();
    }

    void SetImGuizmoTranslate()
    {
        _translate = !_translate;
        _rotate = false;
        _scale = false;
    }

    void SetImGuizmoRotate()
    {
        _translate = false;
        _rotate = !_rotate;
        _scale = false;
    }

    void SetImGuizmoScale()
    {
        _translate = false;
        _rotate = false;
        _scale = !_scale;
    }

private:
    void ResizeCheck()
    {
        if ((_size.x < _prevSize.x || _size.y < _prevSize.y) ||
            (_size.x > _prevSize.x || _size.y > _prevSize.y))
        {
            std::cout << "Scene panel resized: X-" << _size.x << " Y-" << _size.y << std::endl;
            _sceneBuffer.resize((int)_size.x, (int)_size.y);
            _prevSize = _size;
        }
    }

    void RenderFBO()
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

        ImGui::GetWindowDrawList()->AddImage((void *)(_sceneBuffer.texture),
                                             ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y),
                                             ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(),ImGui::GetWindowPos().y + ImGui::GetWindowHeight()),
                                             ImVec2(0, 1), ImVec2(1, 0));
    }

    void UseImGuizmo()
    {
        if (_translate)
        {
            ImGuizmo::Manipulate(glm::value_ptr(_view), glm::value_ptr(_projection),
                                 ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(_model));
        }
        else if (_rotate)
        {
            ImGuizmo::Manipulate(glm::value_ptr(_view), glm::value_ptr(_projection),
                                 ImGuizmo::OPERATION::ROTATE, ImGuizmo::LOCAL, glm::value_ptr(_model));
        }
        else if (_scale)
        {
            ImGuizmo::Manipulate(glm::value_ptr(_view), glm::value_ptr(_projection),
                                 ImGuizmo::OPERATION::SCALE, ImGuizmo::LOCAL, glm::value_ptr(_model));
        }
    }

    FBO& _sceneBuffer;
    glm::mat4& _model;
    glm::mat4& _view;
    glm::mat4& _projection;

    bool _translate;
    bool _rotate;
    bool _scale;

    ImVec2 _prevSize;
};

#endif //OPENGL_GAMEENGINE_SCENEWINDOW_HPP
