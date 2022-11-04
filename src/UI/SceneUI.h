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

            void UI_Gizmo();

        private:
            Ref<Framebuffer> m_Framebuffer;
            Ref<Scene> m_ActiveScene;

            Entity m_HoveredEntity;
            Camera m_Camera;

            bool m_ViewportFocused = false, m_ViewportHovered = false;
 
            glm::vec4 m_BgColor = { 0.3f, 0.4f, 0.5f, 1.0f };
            glm::vec4 m_WireColor = { 0.1f, 0.1f, 0.1f, 1.0f };
            glm::vec4 m_VertexColor = { 1.0f, 0.0f, 0.0f, 1.0f };
            glm::vec4 m_NormalsColor = { 0.0f, 1.0f, 0.0f, 1.0f };
            glm::vec4 m_BoundingBoxColor = { 0.1f, 0.1f, 0.1f, 1.0f };
            
            glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
            glm::vec2 m_ViewportBounds[2];

            int m_GizmoType = -1;

            float m_PointSize = 5;
            float m_LineSize = 5;
            float m_NormalLength = 5;

            bool m_Zbuffer = true;
            bool m_Multisample = true;
            bool m_BackfaceCulling = true;

            bool m_ShowBoundingBox = false;
            bool m_ShowWireFrame = false;
            bool m_ShowNormals = false;
            bool m_ShowVertex = false;
            bool m_Fill = true;

            std::filesystem::path m_CurrentScenePath;

            EntityUI m_SceneEntitiesPanel;
    };
}

