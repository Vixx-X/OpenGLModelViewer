#pragma once

#include "Base.h"
#include <glm/glm.hpp>


namespace GLMV {

    class Mesh
    {
        public:
            Mesh() = default;

            Ref<std::vector<glm::vec3>> Vertices;
            Ref<std::vector<uint32_t>> Indexes;
           // std::vector<Texture> textures;

            static Ref<Mesh> Create() {
                auto ret = CreateRef<Mesh>();
                ret->Vertices = CreateRef<std::vector<glm::vec3>>();
                ret->Indexes = CreateRef<std::vector<uint32_t>>();
                return ret;
            }
        private:

    };

}