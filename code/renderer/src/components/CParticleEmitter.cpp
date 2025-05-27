//
// Created by Cem on 14.07.2022.
//

#include "engine/renderer/components/CParticleEmitter.h"
#include <engine/scene/TreeTraverser.h>


engine::renderer::components::CParticleEmitter::CParticleEmitter(engine::components::id_t id,
                                                                 engine::entities::id_t owner) : CComponent(id, owner)
{

}

void engine::renderer::components::CParticleEmitter::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* particleConstruction =
            dynamic_cast<const engine::renderer::components::CParticleEmitter::ConstructionInfo*>(constructionInfo);
    assert(particleConstruction && "Please use the correct ConstructionInfo: CCamera::ConstructionInfo");

    particleSystem = particle::ParticleSystem(particleConstruction->particleSystemProps);
}

void engine::renderer::components::CParticleEmitter::tick(const float deltaTime)
{
    CComponent::tick(deltaTime);

    particleSystem.tick(deltaTime);
}

void engine::renderer::components::CParticleEmitter::initialVisit(engine::scene::TreeTraverser* traverser)
{

}

void engine::renderer::components::CParticleEmitter::visit(engine::scene::TreeTraverser* traverser)
{
    if(!particleSystem.usesModel())
    {
        shader->use();
        shader->setMatrix("model", traverser->getStackTop());
        shader->setMatrix("view", traverser->getView());
        shader->setMatrix("projection", traverser->getProjection());
        particleSystem.draw(shader);
    }
    else
    {
        meshShader->use();
        meshShader->setMatrix("model", traverser->getStackTop());
        meshShader->setMatrix("view", traverser->getView());
        meshShader->setMatrix("projection", traverser->getProjection());
        particleSystem.draw(meshShader);
    }
}

void engine::renderer::components::CParticleEmitter::activate()
{
    particleSystem.activate();
}

void engine::renderer::components::CParticleEmitter::deactivate()
{
    particleSystem.deactivate();
}
