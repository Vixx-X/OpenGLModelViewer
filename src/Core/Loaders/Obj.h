#pragma once

#include "Base.h"
#include <Core/Scene/Scene.h>

namespace GLMV {

    class ObjLoader
    {
        public:
            static bool Load(const std::string& path, Ref<Scene> scene);
        private:
            static bool LoadMTL(const std::string& path);
    };
}