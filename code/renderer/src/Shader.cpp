//
// Created by coezdemir on 02.11.2020.
//

#include <engine/renderer/Shader.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct glStatusData
{
    int success;
    const char* shaderName;
    char infoLog[GL_INFO_LOG_LENGTH];
};

std::string readText(const std::filesystem::path& fileName)
{
    std::ifstream sourceFile(fileName);
    std::stringstream buffer;
    buffer << sourceFile.rdbuf();
    return buffer.str();
}

engine::renderer::Shader::Shader(const std::filesystem::path& vertexShaderAsset, const std::filesystem::path& fragmentShaderAsset)
{
    vertexShader = loadAndCompileShader(GL_VERTEX_SHADER, vertexShaderAsset);
    fragmentShader = loadAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderAsset);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    {
        glStatusData linkingStatus{};
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkingStatus.success);
        if(!linkingStatus.success)
        {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, linkingStatus.infoLog);
            throw std::runtime_error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string (linkingStatus.infoLog));
        }
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

engine::renderer::Shader::~Shader()
{
    glDeleteProgram(shaderProgram);
}

unsigned int engine::renderer::Shader::loadAndCompileShader(unsigned int shaderType, const std::filesystem::path &shaderAssetPath)
{
    auto shaderSource = readText(shaderAssetPath);
    const char* shaderSrc = shaderSource.c_str();

    unsigned int shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);

    {
        glStatusData compilationStatus{};

        if(shaderType == GL_VERTEX_SHADER) compilationStatus.shaderName = "VERTEX";
        if(shaderType == GL_FRAGMENT_SHADER) compilationStatus.shaderName = "FRAGMENT";

        glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationStatus.success);
        if(!compilationStatus.success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, compilationStatus.infoLog);
            throw std::runtime_error("ERROR::SHADER::" + std::string(compilationStatus.shaderName)
                                                       + "::COMPILATION_FAILED\n"
                                                       + std::string(compilationStatus.infoLog));
        }
    }
    return shader;
}

void engine::renderer::Shader::use() const
{
    glUseProgram(shaderProgram);
}

void engine::renderer::Shader::setMatrix(std::string_view uniform, const glm::mat4 matrix) const
{
    int matrixLoc = glGetUniformLocation(shaderProgram, uniform.data());
    glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void engine::renderer::Shader::setVector(std::string_view uniform, const glm::vec4 vector) const
{
    int vectorLoc = glGetUniformLocation(shaderProgram, uniform.data());
    glUniform4fv(vectorLoc, 1, glm::value_ptr(vector));
}

void engine::renderer::Shader::setVector(std::string_view uniform, const glm::vec3 vector) const
{
    int vectorLoc = glGetUniformLocation(shaderProgram, uniform.data());
    glUniform3fv(vectorLoc, 1, glm::value_ptr(vector));
}

void engine::renderer::Shader::setFloat(std::string_view uniform, const float value) const
{
    int floatLoc = glGetUniformLocation(shaderProgram, uniform.data());
    glUniform1f(floatLoc, value);
}

void engine::renderer::Shader::setTexture(std::string_view uniform, const unsigned int value) const
{
    int textureLoc = glGetUniformLocation(shaderProgram, uniform.data());
    glUniform1i(textureLoc, value);
}

void engine::renderer::Shader::setBool(std::string_view uniform, const bool value) const
{
    int boolLoc = glGetUniformLocation(shaderProgram, uniform.data());
    glUniform1i(boolLoc, (int)value);
}

void engine::renderer::Shader::setVector(std::string_view uniform, glm::vec2 vector) const
{
    int vectorLoc = glGetUniformLocation(shaderProgram, uniform.data());
    glUniform2fv(vectorLoc, 1, glm::value_ptr(vector));
}
