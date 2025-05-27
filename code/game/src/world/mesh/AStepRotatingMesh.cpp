//
// Created by CemOe on 09.12.2021.
//

#include "AStepRotatingMesh.h"

game::world::AStepRotatingMesh::AStepRotatingMesh(const glm::mat4 transform, engine::world::World* const world,
                                                  std::string_view filename,
                                                  engine::scene::components::CStepRotation::ConstructionInfo stepRotationInfo)
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

    stepRotationComponent = rootEntity.addComponent<engine::scene::components::CStepRotation>();
    stepRotationInfo.instantiator = this;
    stepRotationInfo.transformComp = transformComp;
    stepRotationComponent->init(&stepRotationInfo);

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        complexCollisionComp->visit(traverser);
    });
}

void game::world::AStepRotatingMesh::tick(const float deltaTime)
{
    AActor::tick(deltaTime);

    stepRotationComponent->tick(deltaTime);
}
