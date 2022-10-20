#pragma once

#include "Base.h"
#include <filesystem>

#include "Core/Scene/Entity.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Renderer/Camera.h"

namespace GLMV {

    class UILayer
    {
        public:
            UILayer();
            ~UILayer();

            void Render(Timestep timestep);
            void OnEvent(Event& e);

        private:
            bool OnKeyPressed(KeyPressedEvent& e);
            bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

            void NewScene();
            void OpenScene();
            void OpenScene(const std::filesystem::path& path);
            void SaveScene();
            void SaveSceneAs();

            void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

            // UI Panels
            void UI_Toolbar();
        private:
            Ref<Scene> m_ActiveScene;

            Entity m_HoveredEntity;
            Camera m_EditorCamera;

            bool m_ViewportFocused = false, m_ViewportHovered = false;
            glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
            glm::vec2 m_ViewportBounds[2];

            int m_GizmoType = -1;

            bool m_ShowNormals = false;
            bool m_ShowVertex = false;
            bool m_ShowOnlyFrame = false;
    };
}

