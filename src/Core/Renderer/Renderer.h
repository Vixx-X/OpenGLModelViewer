#pragma once

#include "Core/Renderer/Camera.h"
#include "Core/Renderer/Shader.h"
#include "Core/Renderer/VertexArray.h"

namespace GLMV {

    class Renderer
    {
        public:
            static void Init();
            static void Shutdown();

            static void OnWindowResize(uint32_t width, uint32_t height);

            static void BeginScene(Camera& camera);
            static void EndScene();

            static void DrawMesh(const Ref<VertexArray>& vertexArray, const glm::mat4& transform, const glm::vec4& color, const int& id);
            static void DrawLines(const Ref<VertexBuffer>& vertexBuffer, const glm::mat4& transform, const glm::vec4& color, size_t size = 1);
            static void DrawPoints(const Ref<VertexBuffer>& vertexBuffer, const glm::mat4& transform, const glm::vec4& color, size_t size = 1);

            static void SetMultiSample(bool multisample);
            static void SetZBuffer(bool zbuffer);
            static void SetBackfaceCulling(bool backfaceculling);
            static void SetFill(bool fill);
            static void SetClearColor(const glm::vec4& color);
            static void Clear();
        private:
            struct SceneData
            {
                glm::mat4 ViewProjectionMatrix;
            };

            static Scope<SceneData> s_SceneData;
    };
}
