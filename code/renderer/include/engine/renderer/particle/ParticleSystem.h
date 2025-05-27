//
// Created by Cem on 14.07.2022.
//
#pragma once
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include "engine/renderer/Shader.h"
#include "engine/renderer/manager/ModelManager.h"

namespace engine::renderer::particle
{
    struct ParticleSystemProps
    {
        bool bActiveFromStart;
        unsigned int numberOfParticles;
        std::pair<float, float> offsetX;
        std::pair<float, float> offsetY;
        std::pair<float, float> offsetZ;
        std::pair<glm::vec3, glm::vec3> sizeOverLife;
        std::pair<glm::vec3, glm::vec3> velocityMinMax;
        float velocityOverLife;
        std::pair<glm::vec4, glm::vec4> colorOverLife;
        float particleLifetime;
        bool bRecycleOldest;
        bool bUsesTexture;
        std::string_view pathToTexture;
        bool bUsesModel;
        std::string_view modelPath;

        ParticleSystemProps()
            : bActiveFromStart(true),
              numberOfParticles(100),
              offsetX(0.f, 0.f),
              offsetY(0.f, 0.f),
              offsetZ(0.f, 0.f),
              velocityOverLife(1.f),
              particleLifetime(3.f),
              bRecycleOldest(true),
              bUsesTexture(false),
              bUsesModel(false)
        {

        }
    };

    /// \brief Spawns and manages particles
    class ParticleSystem
    {
    private:
        struct Particle
        {
            glm::vec3 position;
            float rotation;
            glm::vec3 size;
            glm::vec3 velocity;
            glm::vec4 color;
            float lifetime;
            float life;
            Particle()
                    : position(0.f),
                      rotation(0.f),
                      size(1.f, 1.f, 1.f),
                      velocity(1.f),
                      color(0.f),
                      lifetime(0.f),
                      life(0.f)
            {

            }
        };

        struct Vertex
        {
            glm::vec3 position;
            glm::vec2 textCoords;

            Vertex(glm::vec3 position, glm::vec2 textCoords)
                    : position(position), textCoords(textCoords)
            {

            }
        };

    public:
        ParticleSystem() = default;

        explicit ParticleSystem(ParticleSystemProps* particleSystemProps);

        ~ParticleSystem();

        // explicit move constructor
        ParticleSystem(ParticleSystem&& other) noexcept
        {
            particleSystemProps = other.particleSystemProps;
            particlePool = other.particlePool;
            bRecycleOldest = other.bRecycleOldest;
            bUsesTexture = other.bUsesTexture;
            texturePath = other.texturePath;
            textureID = other.textureID;
            bUsesModel = other.bUsesModel;
            modelPath = other.modelPath;
            model = other.model;
            oldestParticleIndex = other.oldestParticleIndex;
            nextParticleIndex = other.nextParticleIndex;
            std::swap(this->VAO, other.VAO);
            std::swap(this->VBO, other.VBO);
        }

        ParticleSystem& ParticleSystem::operator=(ParticleSystem&& other) noexcept
        {
            if(this != &other)
            {
                particleSystemProps = other.particleSystemProps;
                particlePool = other.particlePool;
                bRecycleOldest = other.bRecycleOldest;
                bUsesTexture = other.bUsesTexture;
                texturePath = other.texturePath;
                textureID = other.textureID;
                bUsesModel = other.bUsesModel;
                modelPath = other.modelPath;
                model = other.model;
                oldestParticleIndex = other.oldestParticleIndex;
                nextParticleIndex = other.nextParticleIndex;
                std::swap(this->VAO, other.VAO);
                std::swap(this->VBO, other.VBO);
                return *this;
            }
            return *this;
        }

        void tick(float deltaTime);

        /// \brief Draw particles.
        /// \param shader Shader to use to draw the particles.
        void draw(const Shader* shader) const;

        /// \brief If the particle system uses models as particles.
        /// \return True if the particles are represented by models.
        [[nodiscard]] bool usesModel() const;

        void activate();

        void deactivate();

    private:
        void setup();

        /// \brief Emits a new particle.
        void emit();

        /// \brief Update the currently active particles.
        void updateParticles(float deltaTime);

        ParticleSystemProps particleSystemProps;

        bool bIsActive;

        std::vector<Particle> particlePool;

        unsigned int nextParticleIndex, oldestParticleIndex;

        bool bRecycleOldest;

        bool bUsesTexture;

        std::string_view texturePath;

        bool bUsesModel;

        std::string_view modelPath;

        engine::renderer::manager::ModelHandle model;

        unsigned int textureID;

        unsigned int VAO, VBO;
    };
}

