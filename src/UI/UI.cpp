#include "UI.h"
#include "Base.h"

#include "Application.h"
#include "Core/Scene/SceneSerializer.h"

#include "imgui.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "tinyfiledialogs.h"

namespace GLMV {

    char const * lFilterPatterns[2] = { "*.yml", "*.yaml" };

    UI::UI()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");

        m_Camera = Camera(30.0f, 1.778f, 0.1f, 1000.0f);
        NewScene();
    }

    UI::~UI()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void UI::OnEvent(Event& e)
    {
        ImGuiIO& io = ImGui::GetIO();
        /* e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse; */
        /* e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard; */
    }

    void Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void End()
    {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void UI::Render(Timestep timestep)
    {

        if (m_ActiveScene)
        {
            m_ActiveScene->OnUpdate(timestep, m_Camera);
        }

        Begin();

        UI_Menu();
        UI_Toolbar();

        static bool show = true;
        ImGui::ShowDemoWindow(&show);

        End();
    }

    void UI::UI_Menu()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows, 
                // which we can't undo at the moment without finer window depth/z control.
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
                if (ImGui::MenuItem("New", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                    OpenScene();

                if (ImGui::MenuItem("Save", "Ctrl+S"))
                    SaveScene();

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                if (ImGui::MenuItem("Exit")) Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

    }

    void UI::UI_Toolbar()
    {

    }

    void UI::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    }

    void UI::OpenScene()
    {
        std::string filepath = tinyfd_openFileDialog(
                "Open scene...",
                NULL,
                2,
                lFilterPatterns,
                NULL,
                false
                );

        if (!filepath.empty())
            OpenScene(filepath);
    }

    void UI::OpenScene(const std::filesystem::path& path)
    {
        Ref<Scene> newScene = CreateRef<Scene>();
        SceneSerializer serializer(newScene);
        if (serializer.Deserialize(path.string()))
        {
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            /* m_SceneHierarchyPanel.SetContext(m_ActiveScene); */

            m_CurrentScenePath = path;
        }
    }

    void UI::SaveScene()
    {
        if (!m_CurrentScenePath.empty())
            SerializeScene(m_ActiveScene, m_CurrentScenePath);
        else
            SaveSceneAs();
    }

    void UI::SaveSceneAs()
    {
        std::string filepath = tinyfd_saveFileDialog(
                "Save scene...",
                NULL,
                2,
                lFilterPatterns,
                NULL
                );

        if (!filepath.empty())
        {
            SerializeScene(m_ActiveScene, filepath);
            m_CurrentScenePath = filepath;
        }
    }

    void UI::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
    {
        SceneSerializer serializer(scene);
        serializer.Serialize(path.string());
    }
}
