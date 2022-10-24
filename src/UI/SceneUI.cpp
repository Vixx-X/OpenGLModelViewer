#include "SceneUI.h"

#include "Application.h"
#include "Core/Input.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Scene/SceneSerializer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "imgui.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ImGuizmo.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "tinyfiledialogs.h"


namespace GLMV {

    namespace Math {
        bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
        {
            // From glm::decompose in matrix_decompose.inl

            using namespace glm;
            using T = float;

            mat4 LocalMatrix(transform);

            // Normalize the matrix.
            if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
                return false;

            // First, isolate perspective.  This is the messiest.
            if (
                    epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
                    epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
                    epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
            {
                // Clear the perspective partition
                LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
                LocalMatrix[3][3] = static_cast<T>(1);
            }

            // Next take care of translation (easy).
            translation = vec3(LocalMatrix[3]);
            LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

            vec3 Row[3], Pdum3;

            // Now get scale and shear.
            for (length_t i = 0; i < 3; ++i)
                for (length_t j = 0; j < 3; ++j)
                    Row[i][j] = LocalMatrix[i][j];

            // Compute X scale factor and normalize first row.
            scale.x = length(Row[0]);
            Row[0] = detail::scale(Row[0], static_cast<T>(1));
            scale.y = length(Row[1]);
            Row[1] = detail::scale(Row[1], static_cast<T>(1));
            scale.z = length(Row[2]);
            Row[2] = detail::scale(Row[2], static_cast<T>(1));

            // At this point, the matrix (in rows[]) is orthonormal.
            // Check for a coordinate system flip.  If the determinant
            // is -1, then negate the matrix and the scaling factors.
            Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
            if (dot(Row[0], Pdum3) < 0)
            {
                for (length_t i = 0; i < 3; i++)
                {
                    scale[i] *= static_cast<T>(-1);
                    Row[i] *= static_cast<T>(-1);
                }
            }

            rotation.y = asin(-Row[0][2]);
            if (cos(rotation.y) != 0) {
                rotation.x = atan2(Row[1][2], Row[2][2]);
                rotation.z = atan2(Row[0][1], Row[0][0]);
            }
            else {
                rotation.x = atan2(-Row[2][0], Row[1][1]);
                rotation.z = 0;
            }
            return true;
        }

    }

    char const * lFilterPatterns[2] = { "*.yml", "*.yaml" };

    SceneUI::SceneUI()
    {
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_Camera = Camera(30.0f, 1.778f, 0.1f, 1000.0f);
        NewScene();
    }

    SceneUI::~SceneUI()
    {
    }

    void SceneUI::OnEvent(Event& e)
    {
        m_Camera.OnEvent(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(SceneUI::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(SceneUI::OnMouseButtonPressed));
    }

    bool SceneUI::OnKeyPressed(KeyPressedEvent& e)
    {
        // Allow for just first press
        if (e.GetRepeatCount() > 0)
            return false;

        bool control = Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL) || Input::IsKeyPressed(GLFW_KEY_RIGHT_CONTROL);
        bool shift = Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT) || Input::IsKeyPressed(GLFW_KEY_RIGHT_SHIFT);

        switch (e.GetKeyCode())
        {
            case GLFW_KEY_N:
                {
                    if (control)
                        NewScene();

                    break;
                }
            case GLFW_KEY_O:
                {
                    if (control)
                        OpenScene();

                    break;
                }
            case GLFW_KEY_S:
                {
                    if (control && shift)
                        SaveSceneAs();

                    break;
                }

                // Gizmos
            case GLFW_KEY_Q:
                {
                    if (!ImGuizmo::IsUsing())
                        m_GizmoType = -1;
                    break;
                }
            case GLFW_KEY_T:
                {
                    if (!ImGuizmo::IsUsing())
                        m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                    break;
                }
            case GLFW_KEY_R:
                {
                    if (!ImGuizmo::IsUsing())
                        m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                    break;
                }
            case GLFW_KEY_E:
                {
                    if (!ImGuizmo::IsUsing())
                        m_GizmoType = ImGuizmo::OPERATION::SCALE;
                    break;
                }
        }

        return true;
    }

    bool SceneUI::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_1)
        {
            if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(GLFW_KEY_LEFT_ALT))
                m_SceneEntitiesPanel.SetSelectedEntity(m_HoveredEntity);
        }
        return false;
    }

    void SceneUI::OnUpdate(Timestep ts)
    {
        // Resize
        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
                m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
                (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        m_Framebuffer->Bind();
        Renderer::SetClearColor(m_BgColor);
        Renderer::Clear();

        // Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        // Update scene
        m_Camera.OnUpdate(ts);
        m_ActiveScene->OnUpdate(ts, m_Camera);

        auto[mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            auto pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
        }
        
        Renderer::BeginScene(m_Camera);

        auto group = m_ActiveScene.get()->m_Registry.group<TransformComponent, MeshComponent>();
        for (auto entity : group)
        {
            auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

            glPolygonOffset(10, 10);

            // draw points
            if (m_ShowVertex)
                Renderer::DrawPoints(mesh.GetVertex(), transform.GetTransform(), m_VertexColor, mesh.MeshVertex->Vertex->size());

            // draw normals
            if (m_ShowNormals)
            {
                Ref<VertexArray> VertexArray = VertexArray::Create();
                Ref<std::vector<glm::vec3>> vertices = mesh.MeshVertex->Normals;

                for (int i = 1; i < vertices->size(); i += 2)
                    vertices->at(i) *= m_LineSize;

                Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create((float*)vertices->data(), vertices->size() * sizeof(glm::vec3));
                BufferLayout layout = {
                    { ShaderDataType::Float3, "a_Position" },
                };
                vertexBuffer->SetLayout(layout);
                VertexArray->AddVertexBuffer(vertexBuffer);
                Renderer::DrawLines(VertexArray, transform.GetTransform(), m_WireColor, vertices->size() / 2);
            }

            // draw wireframe
            if (m_ShowWireFrame)
            {
                Renderer::SetFill(false);
                Renderer::DrawMesh(mesh.GetWireFrameMesh(), transform.GetTransform(), m_WireColor);
                Renderer::SetFill(m_Fill);
            }
            // draw bounding box
            if (m_ShowBoundingBox)
                Renderer::DrawCube(transform.GetTransform(), m_BoundingBoxColor);

            glPolygonOffset(0, 0);

        }

        Renderer::EndScene();

        m_Framebuffer->Unbind();
    }

    void SceneUI::Render()
    {
        // Note: Switch this to true to enable dockspace
        static bool dockspaceOpen = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // Converting viewport in our window space
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("SceneDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSizeX;

        UI_Menu();
        UI_Stats();

        m_SceneEntitiesPanel.Render();

        // Viewport
        ImGui::Begin("Viewport");

        // Send framebuffer to texture and display it
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiUI()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        UI_Gizmo();
 
        ImGui::End(); // Viewport

        ImGui::End(); // DockSpace
    }

    void SceneUI::UI_Menu()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
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

            if (ImGui::BeginMenu("View"))
            {
                ImGui::ColorEdit3("Background Color", glm::value_ptr(m_BgColor));
                ImGui::ColorEdit3("Wire Color", glm::value_ptr(m_WireColor));
                ImGui::ColorEdit3("Vertex Color", glm::value_ptr(m_VertexColor));
                ImGui::ColorEdit3("Normals Color", glm::value_ptr(m_NormalsColor));
                ImGui::ColorEdit3("Bounding Box Color", glm::value_ptr(m_BoundingBoxColor));

                ImGui::Separator();

                if (ImGui::Checkbox("Z-Buffer", &m_Zbuffer))
                    Renderer::SetZBuffer(m_Zbuffer);
                if (ImGui::Checkbox("Antialiasing", &m_Multisample))
                    Renderer::SetMultiSample(m_Multisample);
                if (ImGui::Checkbox("Back Face Culling", &m_BackfaceCulling))
                    Renderer::SetBackfaceCulling(m_BackfaceCulling);
                if (ImGui::DragFloat("Point Size", &m_PointSize, 1.0f, 1.0f));
                    Renderer::SetPointSize(m_PointSize);
                if (ImGui::DragFloat("Line Size", &m_LineSize, 1.0f, 1.0f));
                    Renderer::SetLineSize(m_LineSize);

                ImGui::Separator();

                ImGui::Checkbox("Show BoundingBox", &m_ShowBoundingBox);
                ImGui::Checkbox("Show WireFrame", &m_ShowWireFrame);
                ImGui::Checkbox("Show Normals", &m_ShowNormals);
                ImGui::Checkbox("Show Vertex", &m_ShowVertex);
                if (ImGui::Checkbox("Fill Triangle", &m_Fill))
                    Renderer::SetFill(m_Fill);

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    void SceneUI::UI_Toolbar()
    {
    }

    void SceneUI::UI_Stats()
    {
        ImGui::Begin("Stats");

        ImGui::Text("Frameraete: %d", (int) ImGui::GetIO().Framerate);

        std::string name = "None";
        if (m_HoveredEntity)
            name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
        ImGui::Text("Hovered Entity: %s", name.c_str());

        std::string id_name = "None";
        if (m_HoveredEntity)
            id_name = std::to_string(m_HoveredEntity.GetComponent<IDComponent>().ID);
        ImGui::Text("Hovered Entity ID: %s", id_name.c_str());
        
        ImGui::End();
    }

    void SceneUI::UI_Gizmo()
    {
        // Gizmos
        Entity selectedEntity = m_SceneEntitiesPanel.GetSelectedEntity();
        if (selectedEntity && m_GizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

            // Camera
            const glm::mat4& cameraProjection = m_Camera.GetProjection();
            glm::mat4 cameraView = m_Camera.GetViewMatrix();

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            // Snapping
            bool snap = Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL);
            float snapValue = 0.5f; // Snap to 0.5m for translation/scale
            // Snap to 45 degrees for rotation
            if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
                snapValue = 45.0f;

            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                nullptr, snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);

                glm::vec3 deltaRotation = rotation - tc.Rotation;
                tc.Translation = translation;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }
    }

    void SceneUI::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneEntitiesPanel.SetScene(m_ActiveScene);
        m_CurrentScenePath = "";
    }

    void SceneUI::OpenScene()
    {
        auto filepath = tinyfd_openFileDialog(
                "Open scene...",
                NULL,
                2,
                lFilterPatterns,
                NULL,
                false
                );

        if (!filepath)
            return;

        OpenScene(std::string(filepath));
    }

    void SceneUI::OpenScene(const std::filesystem::path& path)
    {
        Ref<Scene> newScene = CreateRef<Scene>();
        SceneSerializer serializer(newScene);
        if (serializer.Deserialize(path.string()))
        {
            m_ActiveScene = newScene;
            m_ActiveScene = newScene;
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_SceneEntitiesPanel.SetScene(m_ActiveScene);

            m_CurrentScenePath = path;
        }
    }

    void SceneUI::SaveScene()
    {
        if (!m_CurrentScenePath.empty())
            SerializeScene(m_ActiveScene, m_CurrentScenePath);
        else
            SaveSceneAs();
    }

    void SceneUI::SaveSceneAs()
    {
        auto filepath = tinyfd_saveFileDialog(
                "Save scene...",
                NULL,
                2,
                lFilterPatterns,
                NULL
                );

        if (!filepath)
            return;
        
        auto fpath = std::string(filepath);
        SerializeScene(m_ActiveScene, fpath + ".yaml");
        m_CurrentScenePath = fpath;
    }

    void SceneUI::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
    {
        SceneSerializer serializer(scene);
        serializer.Serialize(path.string());
    }
}
