#pragma once

#include "Core/Core.h"
#include "Core/Timestep.h"
#include "Core/Window.h"
#include "Core/Scene/Scene.h"

#include "UI/UI.h"

namespace GLMV {

    class Application
    {
        public:
            Application(const std::string& name = "GLMV", uint32_t width = 1280, uint32_t height = 720);
            ~Application();

            void Run();

            void OnEvent(Event& e);

            inline Window& GetWindow() { return *m_Window; }

            inline static Application& Get() { return *s_Instance; }

        private:
            bool OnWindowClose(WindowCloseEvent& e);
            bool OnWindowResize(WindowResizeEvent& e);

            std::unique_ptr<Window> m_Window;
            UILayer* m_UILayer;
            Scene* m_SceneLayer;
            bool m_Running = true;
            bool m_Minimized = false;
            float m_LastFrameTime = 0.0f;
            static Application* s_Instance;
    };

}
