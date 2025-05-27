//
// Created by coezdemir on 29.01.2021.
//
#include "AStaticMesh.h"
#include <engine/renderer/Renderer.h>

game::world::AStaticMesh::AStaticMesh(const glm::mat4 transform, engine::world::World* const world,
                                       std::string_view filename)
        : AMyActor(transform, world)
{
    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = filename;
    meshComp->init(&meshInfo);

    // Init mesh collision
    complexCollisionComp = rootEntity.addComponent<engine::physics::components::CComplexCollision>();
    engine::physics::components::CComplexCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.bIsDynamic = false;
    collisionInfo.bIsSimulationShape = true;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bUpdateLocation = false;
    meshComp->getAllVertices(vertices);
    meshComp->getAllIndices(indices);
    collisionInfo.vertices = &vertices;
    collisionInfo.indices = &indices;
    collisionInfo.bActiveFromStart = true;
    complexCollisionComp->init(&collisionInfo);

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
        meshComp->visit(traverser);
        complexCollisionComp->visit(traverser);
    });
}