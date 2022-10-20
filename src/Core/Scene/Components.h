#pragma once

#include "Core/UUID.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace GLMV {

    struct IDComponent
    {
        UUID ID;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
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
        Ref<Mesh> Mesh;

        MeshComponent() = default;
        MeshComponent(const MeshComponent&) = default;
        MeshComponent(const Mesh& Mesh)
            : Mesh(Mesh) {}

        Mesh& GetMesh() const
        {
            return Mesh;
        }
    };

    struct MaterialComponent
    {
        glm::vec4 Color{ 0.7f, 0.7f, 0.7f, 1.0f };

        MaterialComponent() = default;
        MaterialComponent(const MaterialComponent&) = default;
        MaterialComponent(const glm::vec4& color)
            : Color(color) {}
    };
}
