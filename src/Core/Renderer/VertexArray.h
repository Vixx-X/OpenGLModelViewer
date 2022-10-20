#pragma once

#include "Base.h"

namespace GLMV {

    class VertexArray
    {
        public:
            VertexArray();
            virtual ~VertexArray();

            virtual void Bind() const;
            virtual void Unbind() const;

            virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
            virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

            virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
            virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

            static VertexArray* Create() { return VertexArray() }

        private:
            uint32_t m_RendererID;
            uint32_t m_VertexBufferIndex = 0;
            std::vector<Ref<VertexBuffer>> m_VertexBuffers;
            Ref<IndexBuffer> m_IndexBuffer;
    };

}
