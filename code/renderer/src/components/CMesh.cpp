//
// Created by Cem on 26.07.2022.
//

#include "engine/renderer/components/CMesh.h"
#include <engine/scene/TreeTraverser.h>

engine::renderer::components::CMesh::CMesh(engine::components::id_t id, engine::entities::id_t owner)
    : CComponent(id, owner), mesh(), bIsVisible(true)
{

}

void engine::renderer::components::CMesh::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* meshConstruction =
            dynamic_cast<const engine::renderer::components::CMesh::ConstructionInfo*>(constructionInfo);
    assert(meshConstruction && "Please use the correct ConstructionInfo: CCamera::ConstructionInfo");

    mesh = Mesh(meshConstruction->verticesData, meshConstruction->indices,
                meshConstruction->triangleCount, meshConstruction->textures, meshConstruction->material);

}

void engine::renderer::components::CMesh::initialVisit(scene::TreeTraverser* traverser)
{

}

void engine::renderer::components::CMesh::visit(scene::TreeTraverser* traverser)
{
    if(!bIsVisible)
        return;

    glm::mat4 view = traverser->getView();
    glm::mat4 stackTop = traverser->getStackTop();

    shader->use();
    shader->setMatrix("model", stackTop);

    mesh.draw(shader);
}

void engine::renderer::components::CMesh::setMesh(Model& newMesh)
{
    mesh = std::move(newMesh);
}

void engine::renderer::components::CMesh::setVisibility(bool bNewVisibility)
{
    bIsVisible = bNewVisibility;
}

bool engine::renderer::components::CMesh::isVisible() const
{
    return bIsVisible;
}
