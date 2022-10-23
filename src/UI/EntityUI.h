#pragma once

#include "Core/Core.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"

namespace GLMV {

    class EntityUI
    {
        public:
            EntityUI() = default;
            EntityUI(const Ref<Scene>& scene);

            void SetScene(const Ref<Scene>& scene);

            void Render();

            Entity GetSelectedEntity() const { return m_SelectionContext; }
            void SetSelectedEntity(Entity entity);
        private:
            void DrawEntity(Entity entity);
            void DrawComponents(Entity entity);
            void ImportMesh();
        private:
            Ref<Scene> m_Context;
            Entity m_SelectionContext;
    };

}
