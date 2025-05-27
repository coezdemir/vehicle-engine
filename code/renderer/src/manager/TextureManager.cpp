//
// Created by Cem on 15.07.2022.
//
#include <engine/renderer/manager/TextureManager.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

unsigned int engine::renderer::manager::TextureManager::currentlyBound;
std::filesystem::path engine::renderer::manager::TextureManager::texturePath;
std::map<engine::renderer::Texture, unsigned int, engine::renderer::manager::TextureManager::TextureCompare> engine::renderer::manager::TextureManager::textures;

void engine::renderer::manager::TextureManager::init()
{
    currentlyBound = 0;
}

void engine::renderer::manager::TextureManager::shutdown()
{
    for (const auto& entry: textures)
        glDeleteTextures(1, &entry.first.id);

    textures.clear();
}

unsigned int engine::renderer::manager::TextureManager::loadTexture(std::string_view path)
{
    texturePath = TEXTURES_HOME;
    texturePath += path;

    for (auto& entry: textures)
    {
        if (entry.first.path == texturePath)
        {
            ++entry.second;
            return entry.first.id;
        }
    }

    Texture texture;
    texture.id = loadTextureInternal(texturePath.string());
    texture.path = texturePath;
    textures.insert({texture, 1});
    return texture.id;
}

unsigned int engine::renderer::manager::TextureManager::loadTextureInternal(std::string_view path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.data(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_NONE;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else
    {
        std::cout << stbi_failure_reason() << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void
engine::renderer::manager::TextureManager::bindTexture(const engine::renderer::Shader* shader,
                                                       std::string_view textureName,
                                                       unsigned int textureID, bool resetBoundTexture)
{
    currentlyBound = resetBoundTexture ? 0 : ++currentlyBound;
    glActiveTexture(GL_TEXTURE0 + currentlyBound);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->setTexture(textureName, GL_TEXTURE0 + currentlyBound);
}

void engine::renderer::manager::TextureManager::lostUser(unsigned int id)
{
    if (textures.empty())
        return;

    for (auto it = textures.begin(); it != textures.end();)
    {
        if (it->first.id == id)
        {
            textures.erase(it);
            glDeleteTextures(1, &id);
            return;
        }
        else
            ++it;
    }
}
