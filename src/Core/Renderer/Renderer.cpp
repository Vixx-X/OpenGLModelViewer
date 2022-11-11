#include "Renderer.h"

#include <glad/glad.h>

namespace GLMV {

    static Ref<Shader> s_TriangleShader, s_DefaultShader;
    Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
    static bool s_Fill = true;

    void Renderer::Init()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        s_TriangleShader = Shader::Create("assets/shaders/Mesh.glsl");
        s_DefaultShader = Shader::Create("assets/shaders/Default.glsl");
    }

    void Renderer::Shutdown()
    {
    }

    void Renderer::BeginScene(Camera& camera)
    {
        s_SceneData->ViewProjectionMatrix = camera.GetViewProjection();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::DrawMesh(const Ref<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, const int& id)
    {
        s_TriangleShader->Bind();
        s_TriangleShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        s_TriangleShader->UploadUniformMat4("u_Transform", transform);
        s_TriangleShader->UploadUniformFloat4("u_Color", color);
        s_TriangleShader->UploadUniformInt("u_EntityID", id);

        vertexArray->Bind();
        glDrawElements(s_Fill? GL_TRIANGLES : GL_LINE_LOOP, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void Renderer::DrawMesh(const Ref<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color)
    {
        s_DefaultShader->Bind();
        s_DefaultShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        s_DefaultShader->UploadUniformMat4("u_Transform", transform);
        s_DefaultShader->UploadUniformFloat4("u_Color", color);

        vertexArray->Bind();
        glDrawElements(GL_LINE_LOOP, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void Renderer::DrawLines(const Ref<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, size_t size)
    {
        s_DefaultShader->Bind();
        s_DefaultShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        s_DefaultShader->UploadUniformMat4("u_Transform", transform);
        s_DefaultShader->UploadUniformFloat4("u_Color", color);

        vertexArray->Bind();
        glDrawArrays(GL_LINES, 0, size * 2);
    }

    void Renderer::DrawPoints(const Ref<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, size_t size)
    {
        s_DefaultShader->Bind();
        s_DefaultShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        s_DefaultShader->UploadUniformMat4("u_Transform", transform);
        s_DefaultShader->UploadUniformFloat4("u_Color", color);

        vertexArray->Bind();
        glDrawArrays(GL_POINTS, 0, size);
    }

    void Renderer::DrawCube(const glm::mat4& transform, const glm::vec4& color)
    {
        s_DefaultShader->Bind();
        s_DefaultShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        s_DefaultShader->UploadUniformMat4("u_Transform", transform);
        s_DefaultShader->UploadUniformFloat4("u_Color", color);

        float vertices[] =
        { //     UNIT CUBE      COORDINATES         
            0.500000, -0.500000, -0.500000,
            0.500000, -0.500000,  0.500000,
           -0.500000, -0.500000,  0.500000,
           -0.500000, -0.500000, -0.500000,
            0.500000,  0.500000, -0.500000,
            0.500000,  0.500000,  0.500000,
           -0.500000,  0.500000,  0.500000,
           -0.500000,  0.500000, -0.500000,
        };

        uint32_t indices[] =
        {
            1,2,3,
            7,6,5,
            4,5,1,
            5,6,2,
            2,6,7,
            0,3,7,
            0,1,3,
            4,7,5,
            0,4,1,
            1,5,2,
            3,2,7,
            4,0,7
        };

        auto vertexArray = VertexArray::Create();
        Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
        vertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            });
        vertexArray->AddVertexBuffer(vertexBuffer);

        Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices));
        vertexArray->SetIndexBuffer(indexBuffer);

        vertexArray->Bind();
        glDrawElements(GL_LINE_LOOP, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void Renderer::SetFill(bool fill)
    {
        s_Fill = fill;
    }

    void Renderer::SetPointSize(float size)
    {
        glPointSize(size);
    }

    void Renderer::SetLineSize(float size)
    {
        glLineWidth(size);
    }

    void Renderer::SetMultiSample(bool multisample)
    {
        if (multisample)
            glEnable(GL_MULTISAMPLE);
        else
            glDisable(GL_MULTISAMPLE);
    }

    void Renderer::SetZBuffer(bool zbuffer)
    {
        glDepthMask(zbuffer);
    }

    void Renderer::SetBackfaceCulling(bool backfaceculling)
    {
        if (backfaceculling)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }

    void Renderer::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        glViewport(0, 0, width, height);
    }
}
