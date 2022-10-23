#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace YAML {

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

}

namespace GLMV {

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
        : m_Scene(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        GLMV_ASSERT(entity.HasComponent<IDComponent>());

        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

        if (entity.HasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // TagComponent

            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;

            out << YAML::EndMap; // TagComponent
        }

        if (entity.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            auto& tc = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

            out << YAML::EndMap; // TransformComponent
        }

        if (entity.HasComponent<MeshComponent>())
        {
            out << YAML::Key << "MeshComponent";
            out << YAML::BeginMap; // MeshComponent

            auto& tc = entity.GetComponent<MeshComponent>();
            out << YAML::Key << "Name" << YAML::Value << tc.Name;

            out << YAML::Key << "Vertices" << YAML::Value << YAML::Flow << YAML::BeginSeq;
            for (int idx = 0; idx < tc.MeshVertex->Vertices->size(); idx += 2) // pos, norm
            {
                out << tc.MeshVertex->Vertices->at(idx);
                out << tc.MeshVertex->Vertices->at(idx + 1);
            }
            out << YAML::EndSeq;

            out << YAML::Key << "Indexes" << YAML::Value << YAML::Flow << YAML::BeginSeq;
            for (int idx = 0; idx < tc.MeshVertex->Indexes->size(); ++idx)
            {
                out << tc.MeshVertex->Indexes->at(idx);
            }
            out << YAML::EndSeq;

            out << YAML::EndMap; // MeshComponent
        }

        if (entity.HasComponent<MaterialComponent>())
        {
            out << YAML::Key << "MaterialComponent";
            out << YAML::BeginMap; // MaterialComponent

            auto& tc = entity.GetComponent<MaterialComponent>();
            out << YAML::Key << "Name" << YAML::Value << tc.Name;
            out << YAML::Key << "Color" << YAML::Value << tc.Color;
            out << YAML::EndMap; // MaterialComponent
        }

        out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const std::string& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_Scene->m_Registry.each([&](auto entityID)
        {
            Entity entity = { entityID, m_Scene.get() };
            if (!entity)
                return;

            SerializeEntity(out, entity);
        });
        out << YAML::EndSeq;

        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filepath);
        }
        catch (YAML::ParserException e)
        {
            LOG_ERROR("Failed to load .yaml file '%s'\n", filepath.c_str());
            return false;
        }

        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();

                std::string tag;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent)
                    tag = tagComponent["Tag"].as<std::string>();

                Entity deserializedEntity = m_Scene->CreateEntityWithUUID(tag, uuid);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    auto& tc = deserializedEntity.GetComponent<TransformComponent>(); // there is already in there
                    tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    tc.Scale = transformComponent["Scale"].as<glm::vec3>();
                }

                auto meshComponent = entity["MeshComponent"];
                if (meshComponent)
                {
                    auto& tc = deserializedEntity.AddComponent<MeshComponent>();
                    tc.Name = meshComponent["Name"].as<std::string>();

                    tc.MeshVertex = Mesh::Create();

                    auto vertices = meshComponent["Vertices"];
                    if (vertices)
                    {
                        auto& vx = tc.MeshVertex->Vertices;
                        for (auto vertex : vertices)
                        {
                            vx->push_back(vertex.as<glm::vec3>());
                        }
                    }

                    auto indexes = meshComponent["Indexes"];
                    if (indexes)
                    {
                        auto& idx = tc.MeshVertex->Indexes;
                        for (auto index : indexes)
                        {
                            idx->push_back(index.as<uint32_t>());
                        }
                    }
                }

                auto materialComponent = entity["MaterialComponent"];
                if (materialComponent)
                {
                    auto& tc = deserializedEntity.AddComponent<MaterialComponent>();
                    tc.Name = materialComponent["Name"].as<std::string>();
                    tc.Color = materialComponent["Color"].as<glm::vec4>();
                }

            }
        }

        return true;
    }
}
