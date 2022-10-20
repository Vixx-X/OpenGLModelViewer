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


    Entity Scene::CreateEntity()
    {
        return CreateEntityWithUUID(UUID());
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid)
    {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::OnUpdate(Timestep ts, Camera& camera)
    {
        Renderer::BeginScene(camera);

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto [transform, mesh, material] = group.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

            Renderer::DrawMesh(transform.GetTransform(), mesh.GetMesh(), material.Color);
        }

        Renderer::EndScene();
    }


    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;
    }
}
