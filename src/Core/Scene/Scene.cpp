#include "Scene.h"

#include "Components.h"
#include "Core/Renderer/Renderer.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace GLMV {

    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    Entity Scene::CreateEntity(std::string& name)
    {
        return CreateEntityWithUUID(name, UUID());
    }

    Entity Scene::CreateEntityWithUUID(std::string& name, UUID uuid, UUID guid)
    {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<IDComponent>(uuid, guid);
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Unnamed Entity" : name;
        return entity;
    }

    Entity Scene::CreateEntityWithGroupUUID(std::string& name, UUID uuid)
    {
        Entity entity = CreateEntityWithUUID(name, UUID(), uuid);
        return entity;
    }


    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::OnUpdate(Timestep ts, Camera& camera)
    {
        Renderer::BeginScene(camera);

        auto group = m_Registry.group<TransformComponent, MeshComponent, MaterialComponent>();
        for (auto entity : group)
        {
            auto [transform, mesh, material] = group.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

            Renderer::DrawMesh(mesh.GetMesh(), transform.GetTransform(), material.Color, (uint32_t)entity);
        }

        Renderer::EndScene();
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;
    }
}
