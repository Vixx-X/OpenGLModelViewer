#include "Base.h"

#include "Application.h"

#include "Core/Log.h"
#include "Core/Input.h"
#include "Core/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace GLMV {
    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name, uint32_t width, uint32_t height)
    {
        GLMV_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = Window::Create({ name, width, height });
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        Renderer::Init();

        m_ImGuiUI = new ImGuiUI();
        m_SceneUI = new SceneUI();
    }

    Application::~Application()
    {
        delete m_SceneUI;
        delete m_ImGuiUI;

        Renderer::Shutdown();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

        m_ImGuiUI->OnEvent(e);

        if (e.Handled)
            return;

        m_SceneUI->OnEvent(e);
    }

    void Application::Run()
    {
        while (m_Running)
        {
            float time = (float)glfwGetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized)
            {
                m_SceneUI->OnUpdate(timestep);
                m_ImGuiUI->OnUpdate(timestep);

                m_ImGuiUI->Begin();
                m_SceneUI->Render();
                m_ImGuiUI->Render();
                m_ImGuiUI->End();
            }

            m_Window->OnUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        Close();
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

        return false;
    }
}
