#pragma once

#include "Core/UUID.h"
#include "Core/Renderer/VertexArray.h"
#include "Core/Renderer/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <filesystem>

namespace GLMV {

    struct IDComponent
    {
        UUID ID;
        UUID GroupID = 0;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
    };
    
    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}
    };

    struct TransformComponent
    {
        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation)
            : Translation(translation) {}

        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    struct MeshComponent
    {
        Ref<Mesh> MeshVertex;
        std::filesystem::path Filepath;
        std::string Name;

        MeshComponent() = default;
        MeshComponent(const MeshComponent&) = default;
        MeshComponent(const Ref<Mesh>& mesh, std::string name, std::filesystem::path path = "")
            : MeshVertex(mesh), Name(name), Filepath(path) {}

        Ref<VertexArray> GetMesh() const
        {
#if 0
            Ref<VertexArray> VertexArray = VertexArray::Create();
            Ref<std::vector<glm::vec3>> vertices = MeshVertex->Vertices;
            Ref<std::vector<uint32_t>> indices = MeshVertex->Indexes;

            Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create((float*)vertices->data(), vertices->size());
            BufferLayout layout = {
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" }
            };
            vertexBuffer->SetLayout(layout);
            VertexArray->AddVertexBuffer(vertexBuffer);

            Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices->data(), indices->size());
            VertexArray->SetIndexBuffer(indexBuffer);
            VertexArray->SetIndexBuffer(indexBuffer);

            return VertexArray;
#else
             // Piramid
         //  float vertices[] =
         //  { //  COORDINATES     NORMALS  //
         //      -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,
         //      -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,
         //       0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,
         //       0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,
         //       0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f
         //  };
         //
         //  uint32_t indices[] =
         //  {
         //      0, 1, 2,
         //      0, 2, 3,
         //      0, 1, 4,
         //      1, 2, 4,
         //      2, 3, 4,
         //      3, 0, 4
         //  };

             // Cube
            float vertices[] =
            { //           COORDINATES         //          NORMALS              //
                1.000000, -1.000000, -1.000000,  1.000000, -1.000000, -1.000000,
                1.000000, -1.000000,  1.000000,  1.000000, -1.000000,  1.000000,
               -1.000000, -1.000000,  1.000000, -1.000000, -1.000000,  1.000000,
               -1.000000, -1.000000, -1.000000, -1.000000, -1.000000, -1.000000,
                1.000000,  1.000000, -1.000000,  1.000000,  1.000000, -1.000000,
                1.000000,  1.000000,  1.000000,  1.000000,  1.000000,  1.000000,
               -1.000000,  1.000000,  1.000000, -1.000000,  1.000000,  1.000000,
               -1.000000,  1.000000, -1.000000, -1.000000,  1.000000, -1.000000,
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

            auto testVA = VertexArray::Create();
            Ref<VertexBuffer> testVB = VertexBuffer::Create(vertices, sizeof(vertices));
            testVB->SetLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" }
                });
            testVA->AddVertexBuffer(testVB);

            Ref<IndexBuffer> testIB = IndexBuffer::Create(indices, sizeof(indices));
            testVA->SetIndexBuffer(testIB);
            return testVA;
#endif
        }
    };

    struct MaterialComponent
    {
        glm::vec4 Color{ 0.7f, 0.7f, 0.7f, 1.0f };
        std::string Name;

        MaterialComponent() = default;
        MaterialComponent(const MaterialComponent&) = default;
        MaterialComponent(const glm::vec4 color, std::string Name = "Unnamed Mesh")
            : Color(color), Name(Name) {}
    };
}
