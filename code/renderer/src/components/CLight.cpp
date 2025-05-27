//
// Created by CemOe on 23.08.2021.
//

#include <engine/renderer/components/CLight.h>
#include <engine/scene/SceneGraph.h>
#include <engine/renderer/Renderer.h>
#include <engine/scene/TreeTraverser.h>

engine::renderer::components::CLight::CLight(engine::components::id_t id, entities::id_t owner)
        : CComponent(id, owner), shader(nullptr)
{

}

void engine::renderer::components::CLight::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* lightConstruction =
            dynamic_cast<const engine::renderer::components::CLight::ConstructionInfo*>(constructionInfo);
    assert(lightConstruction && "Please use the correct ConstructionInfo: CCamera::ConstructionInfo");

    vector = lightConstruction->vector;
    ambient = lightConstruction->ambient;
    diffuse = lightConstruction->diffuse;
    specular = lightConstruction->specular;
}

engine::renderer::components::CLight::ConstructionInfo engine::renderer::components::CLight::getLightProperties() const
{
    return {vector, ambient, diffuse, specular};
}

void engine::renderer::components::CLight::initialVisit(scene::TreeTraverser* traverser)
{
    bInitialTraversed = true;

    shader->use();
    shader->setVector("light.vector", vector);
    shader->setVector("light.ambient", ambient);
    shader->setVector("light.diffuse", diffuse);
    shader->setVector("light.specular", specular);
}

void engine::renderer::components::CLight::visit(scene::TreeTraverser* traverser)
{
    if (!bInitialTraversed)
    {
        initialVisit(traverser);
        return;
    }

    shader->use();
    shader->setMatrix("model", traverser->getStackTop());
}

