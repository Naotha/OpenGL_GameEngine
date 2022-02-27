#ifndef OPENGL_GAMEENGINE_PARAMETERSWIDGET_HPP
#define OPENGL_GAMEENGINE_PARAMETERSWIDGET_HPP

#include "Gui/EditorWidget.h"
#include "Gui/SceneWidget.hpp"
#include "Engine/Entity.hpp"
#include <imgui/imgui.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <imgui_filebrowser/imfilebrowser.h>

class ParametersWidget : public EditorWidget{
public:
    ParametersWidget(SceneWidget* sceneWindow, Entity* entity) : EditorWidget("Parameters"), _sceneWindow(sceneWindow),
        _entity(entity)
    {
        _fileDialog.SetTitle("Load Model...");
        _fileDialog.SetTypeFilters({ ".gltf", ".obj" });
        _isModelLoaded = false;
    }

    virtual void Render() final
    {
        Begin();

        RenderButtons();
        RenderFileDialog();
        CheckModelLoaded();

        End();
    }

    bool IsModelLoaded()
    {
        return _isModelLoaded;
    }

    std::string GetModelPath()
    {
        return  _modelPath;
    }

    void ClearSelection()
    {
        _isModelLoaded = false;
    }

    void SetEntity(Entity* entity)
    {
        _entity = entity;
    }

private:
    void RenderButtons()
    {
        if (ImGui::Button("Translate"))
        {
            _sceneWindow->SetImGuizmoTranslate();
        }
        if (ImGui::Button("Rotate"))
        {
            _sceneWindow->SetImGuizmoRotate();
        }
        if (ImGui::Button("Scale"))
        {
            _sceneWindow->SetImGuizmoScale();
        }
        if (ImGui::Button("Reset"))
        {
            _sceneWindow->ResetImGuizmoTransforms();
        }
    }

    void RenderFileDialog()
    {
        if (ImGui::Button("Load Model"))
        {
            _fileDialog.Open();
        }
        _fileDialog.Display();
    }

    void CheckModelLoaded()
    {
        if (_fileDialog.HasSelected())
        {
            _modelPath = _fileDialog.GetSelected().string();
            std::cout<<_modelPath<<std::endl;
            std::replace( _modelPath.begin(), _modelPath.end(), '\\', '/');
            _fileDialog.ClearSelected();
            _isModelLoaded = true;
        }
    }

    SceneWidget* _sceneWindow;
    Entity* _entity;
    ImGui::FileBrowser _fileDialog;
    std::string _modelPath;
    bool _isModelLoaded;
};

#endif //OPENGL_GAMEENGINE_PARAMETERSWIDGET_HPP
