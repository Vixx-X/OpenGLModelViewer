#pragma once

#include "Scene.h"

namespace GLMV {

    class SceneSerializer
    {
        public:
            SceneSerializer(const Ref<Scene>& scene);

            void Serialize(const std::string& filepath);

            bool Deserialize(const std::string& filepath);
        private:
            Ref<Scene> m_Scene;
    };

}
