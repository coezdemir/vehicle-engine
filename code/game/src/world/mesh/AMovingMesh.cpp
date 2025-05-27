//
// Created by CemOe on 02.11.2021.
//

#include "AMovingMesh.h"

game::world::AMovingMesh::AMovingMesh(const glm::mat4 transform, engine::world::World* const world,
                                       std::string_view filename,
                                       engine::scene::components::CTranslation::ConstructionInfo translationInfo)
        : AStaticMesh(transform, world, filename)
{
    // Init mesh collision
    complexCollisionComp = rootEntity.addComponent<engine::physics::components::CComplexCollision>();
    engine::physics::components::CComplexCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.bIsDynamic = true;
    collisionInfo.bIsKinematic = true;
    collisionInfo.bIsSimulationShape = true;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bActiveFromStart = true;
    meshComp->getAllVertices(vertices);
    meshComp->getAllIndices(indices);
    collisionInfo.vertices = &vertices;
    collisionInfo.indices = &indices;
    complexCollisionComp->init(&collisionInfo);

    translationComponent = rootEntity.addComponent<engine::scene::components::CTranslation>();
    translationInfo.instantiator = this;
    translationInfo.transformComp = transformComp;
    translationComponent->init(&translationInfo);
}

void game::world::AMovingMesh::tick(const float deltaTime)
{
    AActor::tick(deltaTime);

    translationComponent->tick(deltaTime);
}
