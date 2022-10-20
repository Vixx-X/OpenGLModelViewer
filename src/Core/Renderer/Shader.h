#pragma once

#include "Base.h"
#include <glm/glm.hpp>

typedef unsigned int GLenum;

namespace GLMV {

    class Shader : public Shader
    {
        public:
            Shader(const std::string& filepath);
            Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
            virtual ~Shader();

            static Shader* Create(const std::string& filepath) { return new Shader(filepath); }
            static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc) { return new Shader(vertexSrc, fragmentSrc); }

            virtual void Bind() const;
            virtual void Unbind() const;

            void UploadUniformInt(const std::string& name, int value);

            void UploadUniformFloat(const std::string& name, float value);
            void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
            void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
            void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

            void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
            void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
        private:
            std::string ReadFile(const std::string& filepath);
            std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
            void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
        private:
            uint32_t m_RendererID;
    };

}
