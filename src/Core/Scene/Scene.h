#pragma once

#include "Core/Timestep.h"
#include "Core/Renderer/Camera.h"
#include "Core/UUID.h"

#include "entt.hpp"

namespace GLMV {

    class Entity;

    class Scene
    {
        public:
            Scene();
            ~Scene();

            Entity CreateEntity();
            Entity CreateEntityWithUUID(UUID uuid);
            void DestroyEntity(Entity entity);

            void OnUpdate(Timestep ts, Camera& camera);
            void OnViewportResize(uint32_t width, uint32_t height);
        private:
            entt::registry m_Registry;
            uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

            friend class Entity;
            friend class SceneSerializer;
    };

}
