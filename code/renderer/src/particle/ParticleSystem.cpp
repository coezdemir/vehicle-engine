//
// Created by Cem on 14.07.2022.
//
#include "engine/renderer/particle/ParticleSystem.h"
#include "glad/glad.h"
#include "engine/math/Math.h"
#include "engine/renderer/manager/TextureManager.h"
#include <engine/random/Random.h>
#include <utility>

engine::renderer::particle::ParticleSystem::ParticleSystem(
        engine::renderer::particle::ParticleSystemProps* particleSystemProps)
        : particleSystemProps(std::move(*particleSystemProps)), nextParticleIndex(0), oldestParticleIndex(0),
          bRecycleOldest(true)
{
    setup();
}

engine::renderer::particle::ParticleSystem::~ParticleSystem()
{

}

void engine::renderer::particle::ParticleSystem::setup()
{
    // Setup
    float vertices[] = {
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Fill mesh buffer
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set attributes
    const GLuint attributeLocationVertexPosition = 0;
    glEnableVertexAttribArray(attributeLocationVertexPosition);
    glVertexAttribPointer(attributeLocationVertexPosition, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), (void*) 0);

    const GLuint attributeLocationTextCoordPosition = 1;
    glEnableVertexAttribArray(attributeLocationTextCoordPosition);
    glVertexAttribPointer(attributeLocationTextCoordPosition, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), (void*) (2 * sizeof(float)));
    glBindVertexArray(0);

    bIsActive = particleSystemProps.bActiveFromStart;

    bRecycleOldest = particleSystemProps.bRecycleOldest;

    bUsesTexture = particleSystemProps.bUsesTexture;

    texturePath = particleSystemProps.pathToTexture;

    // texture ID become 0
    if (bUsesTexture)
        textureID = manager::TextureManager::loadTexture(texturePath);

    bUsesModel = particleSystemProps.bUsesModel;

    modelPath = particleSystemProps.modelPath;

    if (bUsesModel)
        model = manager::ModelManager::loadModel(modelPath, true);

    // resize() instead of reserve() so we can initialize the particles
    particlePool.resize(particleSystemProps.numberOfParticles);
}

void engine::renderer::particle::ParticleSystem::tick(const float deltaTime)
{
    if(!bIsActive)
        return;

    emit();

    updateParticles(deltaTime);
}

void engine::renderer::particle::ParticleSystem::emit()
{
    Particle& particle = particlePool[nextParticleIndex];

    if(!bRecycleOldest && nextParticleIndex == oldestParticleIndex && particle.life > 0.f)
        return;

    // Pos, Rot, Scale
    float x = engine::random::range(particleSystemProps.offsetX.first, particleSystemProps.offsetX.second);
    float y = engine::random::range(particleSystemProps.offsetY.first, particleSystemProps.offsetY.second);
    float z = engine::random::range(particleSystemProps.offsetZ.first, particleSystemProps.offsetZ.second);
    particle.position = glm::vec3(x, y, z);
    particle.rotation = engine::random::range(0.f, 1.f) * 2.f * glm::pi<float>(); // hard coded for now
    particle.size = particleSystemProps.sizeOverLife.first;

    // Velocity
    particle.velocity = particleSystemProps.velocityMinMax.first;
    float velocityVariationX = particleSystemProps.velocityMinMax.second.x - particleSystemProps.velocityMinMax.first.x;
    float velocityVariationY = particleSystemProps.velocityMinMax.second.y - particleSystemProps.velocityMinMax.first.y;
    float velocityVariationZ = particleSystemProps.velocityMinMax.second.z - particleSystemProps.velocityMinMax.first.z;
    particle.velocity += glm::vec3(velocityVariationX * (engine::random::range(0.f, 0.5f) - 0.5f),
                                   velocityVariationY * (engine::random::range(0.f, 0.5f) - 0.5f),
                                   velocityVariationZ * (engine::random::range(0.f, 0.5f) - 0.5f));

    // Color
    particle.color = particleSystemProps.colorOverLife.first;

    // Life
    particle.lifetime = particleSystemProps.particleLifetime;
    particle.life = particle.lifetime;

    nextParticleIndex = ++nextParticleIndex % particlePool.size();
}

void engine::renderer::particle::ParticleSystem::updateParticles(const float deltaTime)
{
    bool bIsRunningOutOfBounds = false;

    if(bRecycleOldest)
    {
        for (Particle* particle = &particlePool[oldestParticleIndex];
             particle != &particlePool[nextParticleIndex] ;
             ++particle)
        {
            if (bIsRunningOutOfBounds)
            {
                if (!nextParticleIndex)
                    return;

                particle = &particlePool[0];
                bIsRunningOutOfBounds = false;
            }

            if (particle == &particlePool[particlePool.size() - 1])
                bIsRunningOutOfBounds = true;

            if (particle->life <= 0.f)
            {
                oldestParticleIndex = ++oldestParticleIndex % particlePool.size();
                continue;
            }

            particle->life -= deltaTime;
            particle->position += particle->velocity * deltaTime;
            particle->rotation += 0.01f * deltaTime;
            particle->velocity = particle->velocity * particleSystemProps.velocityOverLife;
            particle->color = engine::math::lerp(particleSystemProps.colorOverLife.first,
                                                 particleSystemProps.colorOverLife.second,
                                                 particle->life / particle->lifetime);
            particle->size = engine::math::lerp(particleSystemProps.sizeOverLife.first,
                                                particleSystemProps.sizeOverLife.second,
                                                particle->life / particle->lifetime);
        }
    }
    else
    {
        for (Particle& particle : particlePool)
        {
            if (particle.life <= 0.f)
            {
                oldestParticleIndex = ++oldestParticleIndex % particlePool.size();
                continue;
            }

            particle.life -= deltaTime;
            particle.position += particle.velocity * deltaTime;
            particle.rotation += 0.01f * deltaTime;
            particle.velocity = particle.velocity * particleSystemProps.velocityOverLife;
            particle.color = engine::math::lerp(particleSystemProps.colorOverLife.first,
                                                 particleSystemProps.colorOverLife.second,
                                                particle.life / particle.lifetime);
            particle.size = engine::math::lerp(particleSystemProps.sizeOverLife.first,
                                                particleSystemProps.sizeOverLife.second,
                                               particle.life / particle.lifetime);
        }
    }
}

void engine::renderer::particle::ParticleSystem::draw(const engine::renderer::Shader* shader) const
{
    if(!bIsActive)
        return;

    glBlendFunc(GL_SRC_ALPHA, GL_NONE);

    // We need to bind texture only once
    shader->use();
    shader->setBool("bUsesTexture", bUsesTexture);

    if (bUsesTexture)
        manager::TextureManager::bindTexture(shader, "sprite", textureID);

    /*
     * TODO: Lets turn this to instanced rendering -> create an arrays with the attributes and call glDrawArraysInstanced
     * with the use of gl_InstanceID you can get the ID of the instance in the shader
    */
    if(bRecycleOldest)
    {
        bool bIsRunningOutOfBounds = false;
        for (const Particle* particle = &particlePool[oldestParticleIndex];
             particle != &particlePool[nextParticleIndex];
             ++particle)
        {
            if (bIsRunningOutOfBounds)
            {
                if (!nextParticleIndex)
                {
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    return;
                }

                particle = &particlePool[0];
                bIsRunningOutOfBounds = false;
            }

            if (particle == &particlePool[particlePool.size() - 1])
                bIsRunningOutOfBounds = true;

            shader->setVector("offset", particle->position);
            shader->setVector("color", particle->color);
            shader->setVector("scale", particle->size);

            if(!bUsesModel)
            {
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
            else
            {
                model->draw(shader);
            }
        }
    }
    else
    {
        for (const Particle& particle : particlePool)
        {
            if (particle.life <= 0.f)
                continue;

            shader->setVector("offset", particle.position);
            shader->setVector("color", particle.color);
            shader->setVector("scale", particle.size);

            if(!bUsesModel)
            {
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
            else
            {
                model->draw(shader);
            }
        }
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool engine::renderer::particle::ParticleSystem::usesModel() const
{
    return bUsesModel;
}

void engine::renderer::particle::ParticleSystem::activate()
{
    bIsActive = true;
}

void engine::renderer::particle::ParticleSystem::deactivate()
{
    bIsActive = false;
}
