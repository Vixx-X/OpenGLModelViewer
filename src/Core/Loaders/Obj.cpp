#include "Obj.h"
#include "Base.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Scene/Components.h"
#include "Core/Scene/Entity.h"
#include <filesystem>
#include <fstream>


namespace GLMV {

    struct Material
    {
        std::string Name;
        glm::vec3 Diffuse;
    };

    struct MeshNode
    {
        Ref<Mesh> Mesh_;
        std::string Material_;
        std::string Name;
    };

    static std::unordered_map<std::string, Ref<Material>> s_MTLMap;
    static std::vector<Ref<MeshNode>> s_MeshVec;

    static glm::vec3 ComputeNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    {
        return glm::normalize(glm::cross(b - a, c - a));
    }

    // return the min/max points of pts
    template< typename Vec >
    std::pair< Vec, Vec > GetExtents(const Vec* pts, size_t stride, size_t count)
    {
        unsigned char* base = (unsigned char*)pts;
        Vec pmin(*(Vec*)base);
        Vec pmax(*(Vec*)base);
        for (size_t i = 0; i < count; ++i, base += stride)
        {
            const Vec& pt = *(Vec*)base;
            pmin = glm::min(pmin, pt);
            pmax = glm::max(pmax, pt);
        }

        return std::make_pair(pmin, pmax);
    }

    // centers geometry around the origin
    // and scales it to fit in a size^3 box
    template< typename Vec >
    void CenterAndScale(Vec* pts, size_t stride, size_t count, const typename Vec::value_type& size)
    {
        typedef typename Vec::value_type Scalar;

        // get min/max extents
        std::pair< Vec, Vec > exts = GetExtents(pts, stride, count);

        // center and scale 
        const Vec center = (exts.first * Scalar(0.5)) + (exts.second * Scalar(0.5f));

        const Scalar factor = size / glm::compMax(exts.second - exts.first);
        unsigned char* base = (unsigned char*)pts;
        for (size_t i = 0; i < count; ++i, base += stride)
        {
            Vec& pt = *(Vec*)base;
            pt = ((pt - center) * factor);
        }
    }

    bool ObjLoader::Load(const std::string& path, Ref<Scene> scene)
    {
        std::filesystem::path filepath = path.c_str();
        std::string parent = filepath.parent_path().u8string(); // Path to material folder

        std::ifstream in(filepath, std::ios::in, std::ios::binary);
        if (!in)
        {
            LOG_ERROR("Could not open obj file '%s'", filepath.u8string().c_str());
            return false;
        }

        s_MTLMap.clear();
        s_MeshVec.clear();

        Ref<std::vector<glm::vec3>> vertices = CreateRef<std::vector<glm::vec3>>();
        Ref<std::vector<glm::vec3>> normals = CreateRef<std::vector<glm::vec3>>();

        Ref<MeshNode> mesh = {};
        std::string line;
        std::string meshName;
        std::string mtl;
        while (std::getline(in, line))
        {
            std::istringstream iss(line);
            std::string token;

            iss >> token;

            if (token.size() and token[0] == '#') continue;
            else if (token == "mtllib")
            {
                std::string mtlib;
                iss >> mtlib;
                if (!LoadMTL(parent + "/" + mtlib))
                    LOG_ERROR("Could not open mtl file '%s'", (parent + mtlib).c_str());
            }
            else if (token == "o" or token == "g")
            {
                iss >> meshName;
            }
            else if (token == "usemtl")
            {
                if (mesh)
                {
                    // still need to compute normals and normalize
                    s_MeshVec.push_back(mesh);
                }
                iss >> mtl;
                mesh = CreateRef<MeshNode>();
                mesh->Mesh_ = Mesh::Create();
                mesh->Name = meshName;
                mesh->Material_ = mtl;
            }
            else if (token == "v")
            {
                float x = 0, y = 0, z = 0, w = 1;
                iss >> x >> y >> z >> w;
                vertices->push_back(glm::vec3(x, y, z));
            }
            else if (token == "f")
            {
                std::string _str;
                uint32_t cnt{};

                while (iss >> _str)
                {
                    std::istringstream ref(_str);
                    std::string vStr, vtStr, vnStr;
                    std::getline(ref, vStr, '/');
                    std::getline(ref, vtStr, '/');
                    std::getline(ref, vnStr, '/');
                    int v = atoi(vStr.c_str());
                   // int vt = atoi(vtStr.c_str());
                   // int vn = atoi(vnStr.c_str());
                   // vt = (vt >= 0 ? vt : texcoords.size() + vt);
                   // vn = (vn >= 0 ? vn : normals.size() + vn);

                    if (cnt >= 3)
                    {
                        // triangularize
                        auto last = mesh->Mesh_->Indexes->at(mesh->Mesh_->Indexes->size() - 1);
                        mesh->Mesh_->Indexes->push_back(mesh->Mesh_->Indexes->at(0));
                        mesh->Mesh_->Indexes->push_back(last);
                    }

                    mesh->Mesh_->Indexes->push_back(v - 1);

                    cnt++;
                }

                GLMV_ASSERT(mesh->Mesh_->Indexes->size() % 3 == 0, "Should have eneded in triangles");
            }
        }

        GLMV_ASSERT(mesh->Mesh_->Indexes->size() % 3 == 0, "Vertex cannot form all triangles");

        if (mesh)
        {
            // still need to compute normals and normalize
            s_MeshVec.push_back(mesh);
        }

        // computing normals
        normals->resize(vertices->size(), glm::vec3());

        // faces
        for (Ref<MeshNode> mesh : s_MeshVec)
        {
            for (int idx = 0; idx < mesh->Mesh_->Indexes->size(); idx += 3)
            {
                int ix = mesh->Mesh_->Indexes->at(idx),
                    iy = mesh->Mesh_->Indexes->at(idx + 1),
                    iz = mesh->Mesh_->Indexes->at(idx + 2);
                glm::vec3 vx = vertices->at(ix),
                    vy = vertices->at(iy),
                    vz = vertices->at(iz);
                glm::vec3 normal = ComputeNormal(vx, vy, vz);
                normals->at(ix) += normal;
                normals->at(iy) += normal;
                normals->at(iz) += normal;
            }
        }

        // scaling
        CenterAndScale(vertices->data(), sizeof(glm::vec3), vertices->size(), 1);

        UUID guid = UUID();
        for (Ref<MeshNode> meshNode : s_MeshVec)
        {
            Ref<std::vector<glm::vec3>> vertexBuffer = CreateRef<std::vector<glm::vec3>>();

            for (auto i = 0; i < meshNode->Mesh_->Indexes->size(); ++i)
            {
                auto& idx = meshNode->Mesh_->Indexes->at(i);
                glm::vec3 vertex = vertices->at(idx), normal = glm::normalize(normals->at(idx));
                vertexBuffer->push_back(vertex);
                vertexBuffer->push_back(normal);
                mesh->Mesh_->Vertex->push_back(vertex);
                mesh->Mesh_->Normals->push_back(vertex);
                mesh->Mesh_->Normals->push_back(vertex + normal);
                idx = i;
            }

            mesh->Mesh_->Vertices = vertexBuffer;
            mesh->Mesh_->BoundingBox = CreateRef<std::pair< glm::vec3, glm::vec3 >>(GetExtents(mesh->Mesh_->Vertex->data(), sizeof(glm::vec3), mesh->Mesh_->Vertex->size()));

            auto entity = scene->Scene::CreateEntityWithGroupUUID(meshNode->Name, guid);
            entity.AddComponent<MeshComponent>(meshNode->Mesh_, meshNode->Name, path);

            auto it = s_MTLMap.find(meshNode->Material_);
            if (it != s_MTLMap.end())
            {
                entity.AddComponent<MaterialComponent>(glm::vec4(it->second->Diffuse, 1), it->second->Name);
            }
            else
            {
                entity.AddComponent<MaterialComponent>();
            } 
        }

        return true;
    }

    bool ObjLoader::LoadMTL(const std::string& path)
    {
        std::filesystem::path filepath = path.c_str();

        std::ifstream in(filepath, std::ios::in, std::ios::binary);
        if (!in)
        {
            return false;
        }

        std::string line;
        std::string mtl;
        Ref<Material> material = {};

        while (std::getline(in, line))
        {
            std::istringstream iss(line);
            std::string token;

            iss >> token;

            if (token.size() and token[0] == '#') continue;
            else if (token == "newmtl")
            {
                if (material)
                {
                    s_MTLMap[mtl] = material;
                }
                iss >> mtl;
                material = CreateRef<Material>();
                material->Name = mtl;
            }
            else if (token == "Kd")
            {
                float r, g, b;
                iss >> r >> g >> b;
                material->Diffuse = {r, g, b};
            }
        }

        if (material)
        {
            s_MTLMap[mtl] = material;
        }
    }

}