#include "EntityUI.h"

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/Scene/Components.h"

namespace GLMV {

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

        m_Context->m_Registry.each([&](auto entityID)
        {
            Entity entity{ entityID , m_Context.get() };
            DrawEntity(entity);
        });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {};

        // Right-click on blank space
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Create Empty Entity"))
                m_Context->CreateEntity();

            ImGui::EndPopup();
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
        auto tag = std::to_string(entity.GetComponent<IDComponent>().ID);

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectionContext = entity;
        }

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
                entityDeleted = true;

            ImGui::EndPopup();
        }

        if (opened)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
            if (opened)
                ImGui::TreePop();
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            m_Context->DestroyEntity(entity);
            if (m_SelectionContext == entity)
                m_SelectionContext = {};
        }
    }

    void EntityUI::DrawComponents(Entity entity)
    {
        {
            auto& component = entity.GetComponent<TransformComponent>();
            ImGui::InputFloat3("Translation", glm::value_ptr(component.Rotation));
            glm::vec3 rotation = glm::degrees(component.Rotation);
            ImGui::InputFloat3("Rotation", glm::value_ptr(rotation));
            component.Rotation = glm::radians(rotation);
            ImGui::InputFloat3("Scale", glm::value_ptr(component.Scale));
        }

        {
            auto& component = entity.GetComponent<MaterialComponent>();
            ImGui::ColorPicker3("Color", glm::value_ptr(component.Color));
        }
    }
}
