#include "VertexArray.h"

#include <glad/glad.h>

namespace GLMV {

    static GLenum ShaderDataTypeToBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case GLMV::ShaderDataType::Float:    return GL_FLOAT;
            case GLMV::ShaderDataType::Float2:   return GL_FLOAT;
            case GLMV::ShaderDataType::Float3:   return GL_FLOAT;
            case GLMV::ShaderDataType::Float4:   return GL_FLOAT;
            case GLMV::ShaderDataType::Mat3:     return GL_FLOAT;
            case GLMV::ShaderDataType::Mat4:     return GL_FLOAT;
            case GLMV::ShaderDataType::Int:      return GL_INT;
            case GLMV::ShaderDataType::Int2:     return GL_INT;
            case GLMV::ShaderDataType::Int3:     return GL_INT;
            case GLMV::ShaderDataType::Int4:     return GL_INT;
            case GLMV::ShaderDataType::Bool:     return GL_BOOL;
        }

        GLMV_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    VertexArray::VertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
    {
        GLMV_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout)
        {
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribPointer(m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)(intptr_t)element.Offset);
            m_VertexBufferIndex++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
    {
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }

}
