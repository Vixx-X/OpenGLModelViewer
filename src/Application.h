#pragma once

#include "Core/Core.h"
#include "Core/Timestep.h"
#include "Core/Window.h"
#include "Core/Scene/Scene.h"

#include "UI/ImGuiUI.h"
#include "UI/SceneUI.h"

namespace GLMV {

    class Application
    {
        public:
            Application(const std::string& name = "GLMV", uint32_t width = 1280, uint32_t height = 720);
            ~Application();

            void Run();

            void OnEvent(Event& e);

            inline Window& GetWindow() { return *m_Window; }
            inline ImGuiUI* GetImGuiUI() { return m_ImGuiUI; }

            inline static Application& Get() { return *s_Instance; }
            void Close() { m_Running = false; }

        private:
            bool OnWindowClose(WindowCloseEvent& e);
            bool OnWindowResize(WindowResizeEvent& e);

            Scope<Window> m_Window;
            ImGuiUI* m_ImGuiUI;
            SceneUI* m_SceneUI;
            bool m_Running = true;
            bool m_Minimized = false;
            float m_LastFrameTime = 0.0f;
            static Application* s_Instance;
    };

}
