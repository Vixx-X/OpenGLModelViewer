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
            Ref<std::vector<glm::vec3>> Normals;
            Ref<std::vector<glm::vec3>> Vertex;
            Ref<std::pair< glm::vec3, glm::vec3 >> BoundingBox;
           // std::vector<Texture> textures;

            static Ref<Mesh> Create() {
                auto ret = CreateRef<Mesh>();
                ret->Vertices = CreateRef<std::vector<glm::vec3>>();
                ret->Indexes = CreateRef<std::vector<uint32_t>>();
                ret->Normals = CreateRef<std::vector<glm::vec3>>();
                ret->Vertex = CreateRef<std::vector<glm::vec3>>();
                ret->BoundingBox = CreateRef<std::pair< glm::vec3, glm::vec3 >>();
                return ret;
            }
        private:

    };

}