#include "EntityUI.h"

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/Scene/Components.h"
#include <tinyfiledialogs.h>
#include "Core/Loaders/Obj.h"

namespace GLMV {

    static char const* lFilterPatterns[1] = { "*.obj" };

    EntityUI::EntityUI(const Ref<Scene>& context)
    {
        SetScene(context);
    }

    void EntityUI::SetScene(const Ref<Scene>& context)
    {
        m_Context = context;
        m_SelectionContext = {};
    }

    void EntityUI::Render()
    {
        ImGui::Begin("Scene Entities");


        if (m_Context)
        {
            m_Context->m_Registry.each([&](auto entityID)
            {
                Entity entity{ entityID , m_Context.get() };
                DrawEntity(entity);
            });

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                m_SelectionContext = {};

            // Right-click on blank space
            if (ImGui::BeginPopupContextWindow(0, 1))
            {
                if (ImGui::MenuItem("Add Entity"))
                    ImportMesh();

                ImGui::EndPopup();
            }

        }

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);
        }

        ImGui::End();
    }

    void EntityUI::SetSelectedEntity(Entity entity)
    {
        m_SelectionContext = entity;
    }

    void EntityUI::DrawEntity(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        std::string uuid = std::to_string(entity.GetUUID());

        bool entityDeleted = false;

        if (ImGui::Selectable((tag + "##" + uuid).c_str(), ImGuiSelectableFlags_AllowDoubleClick))
            m_SelectionContext = entity;


        if (ImGui::Button(("Delete##" + uuid).c_str()))
            entityDeleted = true;

        if (entityDeleted)
        {
            m_Context->DestroyEntity(entity);
            if (m_SelectionContext == entity)
                m_SelectionContext = {};
        }
    }

    void EntityUI::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            std::strncpy(buffer, tag.c_str(), sizeof(buffer));
            if (ImGui::InputText("Name##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        if (entity.HasComponent<TransformComponent>())
        {
            auto& component = entity.GetComponent<TransformComponent>();
            ImGui::InputFloat3("Translation", glm::value_ptr(component.Rotation));
            glm::vec3 rotation = glm::degrees(component.Rotation);
            ImGui::InputFloat3("Rotation", glm::value_ptr(rotation));
            component.Rotation = glm::radians(rotation);
            ImGui::InputFloat3("Scale", glm::value_ptr(component.Scale));
        }

        if (entity.HasComponent<MaterialComponent>())
        {
            auto& component = entity.GetComponent<MaterialComponent>();
            auto& tag = component.Name;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            std::strncpy(buffer, tag.c_str(), sizeof(buffer));
            if (ImGui::InputText("Material Name##Material Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
        }
    }
    void EntityUI::ImportMesh()
    {
        auto filepath = tinyfd_openFileDialog(
            "Load mesh...",
            NULL,
            1,
            lFilterPatterns,
            NULL,
            false
        );

        if (!filepath)
            return;

        ObjLoader::Load(std::string(filepath), m_Context);
    }
}
