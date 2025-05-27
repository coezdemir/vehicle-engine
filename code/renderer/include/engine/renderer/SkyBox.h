//
// Created by CemOe on 17.12.2021.
//
#pragma once
#include <string>
#include <engine/scene/ITraversableObject.h>
#include "Shader.h"

namespace engine::renderer
{
    /// \brief Class for easy set up and drawing of a skybox
    class SkyBox : public engine::scene::ITraversableObject
    {
    public:
        explicit SkyBox(std::filesystem::path pathToTextures);

        void draw() const;

        void initialVisit(scene::TreeTraverser* traverser) override;

        /// \brief Draws the skybox.
        ///
        /// Call this by subscribing to the correct event in CTransform.
        /// \param treeTraverser Traverser traversing the scene graph.
        void visit(scene::TreeTraverser* traverser) override;

    private:
        unsigned int loadCubemap(const std::filesystem::path& pathToTextures);

        const Shader skyboxShader = Shader("../../assets/shaders/skybox/skybox-vertex-shader.glsl",
                                           "../../assets/shaders/skybox/skybox-fragment-shader.glsl");

        float skyboxVertices[108] = {
                // positions
                -1.0f, 1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,

                -1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f
        };

        unsigned int VAO, VBO, cubemapTexture;
    };
}