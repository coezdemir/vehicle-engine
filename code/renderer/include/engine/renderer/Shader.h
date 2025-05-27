//
// Created by CemOe on 15.08.2021.
//
#pragma once
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace engine::renderer
{
    namespace fs = std::filesystem;

    class Shader
    {
    public:

        Shader(const std::filesystem::path &vertexShaderAsset, const std::filesystem::path &fragmentShaderAsset);

        ~Shader();

        Shader(const Shader &shader) = delete;

        // explicit move constructor
        Shader(Shader &&other) noexcept
        {
            std::swap(this->shaderProgram, other.shaderProgram);
            std::swap(this->vertexShader, other.vertexShader);
            std::swap(this->fragmentShader, other.fragmentShader);
        }

        static unsigned int loadAndCompileShader(unsigned int shaderType, const std::filesystem::path &shaderAssetPath);

        void use() const;

        void setMatrix(std::string_view uniform, glm::mat4 matrix) const;

        void setVector(std::string_view uniform, glm::vec4 vector) const;

        void setVector(std::string_view uniform, glm::vec3 vector) const;

        void setVector(std::string_view uniform, glm::vec2 vector) const;

        void setFloat(std::string_view uniform, float value) const;

        void setTexture(std::string_view uniform, unsigned int value) const;

        void setBool(std::string_view uniform, bool value) const;

    private:
        unsigned int shaderProgram = 0;

        unsigned int vertexShader = 0;

        unsigned int fragmentShader = 0;
    };
}
