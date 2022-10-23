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

    void Renderer::DrawLines(const Ref<VertexBuffer>& vertexBuffer, const glm::mat4& transform, const glm::vec4& color, size_t size)
    {
        s_DefaultShader->Bind();
        s_DefaultShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        s_DefaultShader->UploadUniformMat4("u_Transform", transform);
        s_DefaultShader->UploadUniformFloat4("u_Color", color);

        vertexBuffer->Bind();
        glDrawArrays(GL_LINES, 0, size * 2);
    }

    void Renderer::DrawPoints(const Ref<VertexBuffer>& vertexBuffer, const glm::mat4& transform, const glm::vec4& color, size_t size)
    {
        s_DefaultShader->Bind();
        s_DefaultShader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
        s_DefaultShader->UploadUniformMat4("u_Transform", transform);
        s_DefaultShader->UploadUniformFloat4("u_Color", color);

        vertexBuffer->Bind();
        glDrawArrays(GL_POINTS, 0, size);
    }

    void Renderer::SetFill(bool fill)
    {
        s_Fill = fill;
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
            glCullFace(GL_FRONT_AND_BACK);
        else
            glCullFace(GL_FRONT);
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
