//
// Created by CemOe on 17.12.2021.
//
#include <engine/renderer/SkyBox.h>
#include <filesystem>
#include <iostream>
#include <utility>
#include <glad/glad.h>
#include <engine/scene/TreeTraverser.h>
#include "engine/renderer/manager/TextureManager.h"
#include <stb_image.h>

engine::renderer::SkyBox::SkyBox(std::filesystem::path pathToTextures)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    cubemapTexture = loadCubemap(std::move(pathToTextures));

    skyboxShader.use();
    manager::TextureManager::bindTexture(&skyboxShader, "skybox", cubemapTexture);
}

unsigned int engine::renderer::SkyBox::loadCubemap(const std::filesystem::path& pathToTextures)
{
    std::vector<std::string> faces
    {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load((pathToTextures.string() + faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void engine::renderer::SkyBox::draw() const
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void engine::renderer::SkyBox::initialVisit(engine::scene::TreeTraverser* traverser)
{

}

void engine::renderer::SkyBox::visit(engine::scene::TreeTraverser* traverser)
{
    glm::mat4 view = traverser->getView();
    glm::mat4 projection = traverser->getProjection();

    skyboxShader.use();
    skyboxShader.setMatrix("view", glm::mat4(glm::mat3(view)));
    skyboxShader.setMatrix("projection", projection);

    draw();
}