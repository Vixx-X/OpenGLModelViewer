#pragma once

#include "Core/Timestep.h"
#include "Core/Renderer/Camera.h"
#include "Core/Renderer/Shader.h"
#include "Core/UUID.h"

#include "entt.hpp"

namespace GLMV {

    class Entity;

    class Scene
    {
        public:
            Scene();
            ~Scene();

            Entity CreateEntity(std::string& name);
            Entity CreateEntityWithUUID(std::string& name, UUID uuid, UUID guid = 0);
            Entity CreateEntityWithGroupUUID(std::string& name, UUID uuid);
            void DestroyEntity(Entity entity);

            void OnUpdate(Timestep ts, Camera& camera);
            void OnViewportResize(uint32_t width, uint32_t height);
        private:
            entt::registry m_Registry;
            uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
            Ref<Shader> m_shader;

            friend class Entity;
            friend class SceneSerializer;
            friend class EntityUI;
            friend class SceneUI;
    };
}
