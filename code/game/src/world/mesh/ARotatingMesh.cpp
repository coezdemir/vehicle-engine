//
// Created by CemOe on 02.11.2021.
//

#include "ARotatingMesh.h"

game::world::ARotatingMesh::ARotatingMesh(const glm::mat4 transform, engine::world::World* const world,
                                           const char* filename, const glm::vec3 axis, const float stepSize)
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
    meshComp->getAllVertices(vertices);
    meshComp->getAllIndices(indices);
    collisionInfo.vertices = &vertices;
    collisionInfo.indices = &indices;
    collisionInfo.bActiveFromStart = true;
    complexCollisionComp->init(&collisionInfo);

    rotationComponent = rootEntity.addComponent<engine::scene::components::CRotation>();
    engine::scene::components::CRotation::ConstructionInfo rotationInfo;
    rotationInfo.instantiator = this;
    rotationInfo.transformComp = transformComp;
    rotationInfo.axis = axis;
    rotationInfo.stepSize = stepSize;
    rotationComponent->init(&rotationInfo);
}

void game::world::ARotatingMesh::tick(const float deltaTime)
{
    AActor::tick(deltaTime);

    rotationComponent->tick(deltaTime);
}
