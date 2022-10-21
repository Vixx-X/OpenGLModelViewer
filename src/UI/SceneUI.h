#pragma once

#include "UI.h"
#include "Base.h"
#include <filesystem>

#include "Core/Scene/Entity.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Renderer/Camera.h"
#include "Core/Renderer/Framebuffer.h"

#include "EntityUI.h"

namespace GLMV {

    class SceneUI: public UI
    {
        public:
            SceneUI();
            ~SceneUI();

            void OnUpdate(Timestep ts) override;
            void Render() override;
            void OnEvent(Event& e) override;

            Camera& GetCamera() { return m_Camera; }

        private:
            bool OnKeyPressed(KeyPressedEvent& e);
            bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

            void NewScene();
            void OpenScene();
            void OpenScene(const std::filesystem::path& path);
            void SaveScene();
            void SaveSceneAs();

            void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

            /* // UI Panels */
            void UI_Menu();
            void UI_Toolbar();
            void UI_Stats();

        private:
            Ref<Framebuffer> m_Framebuffer;
            Ref<Scene> m_ActiveScene;

            Entity m_HoveredEntity;
            Camera m_Camera;

            bool m_ViewportFocused = false, m_ViewportHovered = false;
            glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
            glm::vec2 m_ViewportBounds[2];

            int m_GizmoType = -1;

            bool m_ShowNormals = false;
            bool m_ShowVertex = false;
            bool m_ShowOnlyFrame = false;

            std::filesystem::path m_CurrentScenePath;

            EntityUI m_SceneEntitiesPanel;
    };
}

