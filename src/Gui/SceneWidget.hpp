#ifndef OPENGL_GAMEENGINE_SCENEWIDGET_HPP
#define OPENGL_GAMEENGINE_SCENEWIDGET_HPP

#include "Gui/EditorWidget.h"
#include "Engine/FBO.hpp"
#include "Engine/Entity.hpp"
#include <imgui/imgui.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class SceneWidget : public EditorWidget{
public:
    SceneWidget(FBO& sceneBuffer, Entity* entity, glm::mat4& view, glm::mat4& projection)
        : EditorWidget("Scene"), _sceneBuffer(sceneBuffer), _entity(entity), _view(view), _projection(projection)
        {
            _model = entity->transform.GetModelMatrix();
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

    void ResetImGuizmoTransforms()
    {
        _entity->transform.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        _entity->transform.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        _entity->transform.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
        _entity->UpdateTransform();
        _model = _entity->transform.GetModelMatrix();
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
            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation = _model[3];
                _entity->transform.SetPosition(translation);
                _entity->UpdateTransform();
                _model = _entity->transform.GetModelMatrix();
            }
        }
        else if (_rotate)
        {
            ImGuizmo::Manipulate(glm::value_ptr(_view), glm::value_ptr(_projection),
                                 ImGuizmo::OPERATION::ROTATE, ImGuizmo::LOCAL, glm::value_ptr(_model));
            if (ImGuizmo::IsUsing())
            {
                glm::vec3 scale;
                glm::quat rotation;
                glm::vec3 translation;
                glm::vec3 skew;
                glm::vec4 perspective;
                glm::decompose(_model, scale, rotation, translation, skew, perspective);
                _entity->transform.SetRotation(rotation);
                _entity->UpdateTransform();
                _model = _entity->transform.GetModelMatrix();
            }
        }
        else if (_scale)
        {
            ImGuizmo::Manipulate(glm::value_ptr(_view), glm::value_ptr(_projection),
                                 ImGuizmo::OPERATION::SCALE, ImGuizmo::LOCAL, glm::value_ptr(_model));
            if (ImGuizmo::IsUsing())
            {
                glm::vec3 scale;
                glm::quat rotation;
                glm::vec3 translation;
                glm::vec3 skew;
                glm::vec4 perspective;
                glm::decompose(_model, scale, rotation, translation, skew, perspective);
                _entity->transform.SetScale(scale);
                _entity->UpdateTransform();
                _model = _entity->transform.GetModelMatrix();
            }
        }
    }

    FBO& _sceneBuffer;
    Entity* _entity;
    glm::mat4 _model;
    glm::mat4& _view;
    glm::mat4& _projection;

    bool _translate;
    bool _rotate;
    bool _scale;

    ImVec2 _prevSize;
};

#endif //OPENGL_GAMEENGINE_SCENEWIDGET_HPP
