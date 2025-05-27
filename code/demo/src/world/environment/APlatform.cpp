//
// Created by Cem on 26.07.2022.
//

#include "APlatform.h"
#include "engine/physics/vehicle/VehicleUtil.h"

demo::world::APlatform::APlatform(glm::mat4 transform, engine::world::World* world) : AActor(world)
{
    // Init transform component
    transformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    cubeScaling = rootEntity.addComponent<engine::scene::components::CTransform>();
    transformInfo.transform = glm::scale(glm::mat4(1.f), glm::vec3(100.f, 10.f, 100.f));
    cubeScaling->init(&transformInfo);
    transformComp->addChild(cubeScaling->getId());

    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "Cube.obj";
    meshComp->init(&meshInfo);


    // Init mesh collision
    collisionComp = rootEntity.addComponent<engine::physics::components::CComplexCollision>();
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

    physx::PxFilterData qryFilterData;
    qryFilterData.word3 = static_cast<physx::PxU32>(engine::physics::vehicle::DRIVABLE_SURFACE);
    collisionInfo.qryFilterData = qryFilterData;

    physx::PxFilterData simulationFilterData;
    simulationFilterData.word0 = engine::physics::vehicle::COLLISION_FLAG_GROUND;
    simulationFilterData.word1 = engine::physics::vehicle::COLLISION_FLAG_GROUND_AGAINST;
    collisionInfo.simulationFilterData = simulationFilterData;

    collisionComp->init(&collisionInfo);

    cubeScaling->event_init.subscribe([this](engine::scene::TreeTraverser* traverser){
        meshComp->initialVisit(traverser);
        collisionComp->initialVisit(traverser);
    });

    cubeScaling->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser){
        meshComp->visit(traverser);
        collisionComp->visit(traverser);
    });
}
