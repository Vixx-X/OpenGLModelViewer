#pragma once

#include "Base.h"
#include "VertexArray.h"

namespace GLMV {

    class Mesh
    {
        public:
            Mesh(Ref<VertexArray> vertexArray)
                : m_VertexArray(vertexArray) {}
            virtual ~Mesh() = default;

            virtual void Bind() const { m_VertexArray.Bind(); }
            virtual void Unbind() const { m_VertexArray.Unbind(); }

        private:
            Ref<VertexArray> m_VertexArray;
            /* textures coming soon*/
    };
}
