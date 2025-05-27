//
// Created by Cem on 26.06.2022.
//

#include "engine/physics/world/AHeightField.h"
#include "engine/physics/vehicle/VehicleUtil.h"
#include "../../../game/src/physics/MyCollisionCallback.h"

engine::physics::world::AHeightField::AHeightField(glm::mat4 transform, engine::world::World* world)
        : AActor(world)
{
    // Init transform component
    transformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();

    physx::PxFilterData qryFilterData;
    qryFilterData.word3 = static_cast<physx::PxU32>(engine::physics::vehicle::DRIVABLE_SURFACE);

    physx::PxFilterData filterData;
    filterData.word0 = engine::physics::vehicle::COLLISION_FLAG_GROUND;
    filterData.word1 = engine::physics::vehicle::COLLISION_FLAG_GROUND_AGAINST;

    // Init height field component
    heightFieldComp = rootEntity.addComponent<engine::physics::components::CHeightField>();
    engine::physics::components::CHeightField::ConstructionInfo heightFieldInfo;
    heightFieldInfo.instantiator = this;
    heightFieldInfo.qryFilterData = qryFilterData;
    heightFieldInfo.simulationFilterData = filterData;
    heightFieldInfo.hfDesc.setToDefault();
    heightFieldInfo.hfDesc.nbColumns = 3;
    heightFieldInfo.hfDesc.nbRows = 3;

    const physx::PxU32 terrainSamples = 4;
    auto* samples = new physx::PxU32[terrainSamples * terrainSamples]; // gets deleted in the component
    for (physx::PxU32 col = 0; col < terrainSamples; ++col)
    {
        for (physx::PxU32 row = 0; row < terrainSamples; ++row)
        {
            physx::PxI16 height;
            height = 0;

            auto& smp = (physx::PxHeightFieldSample&) (samples[(row * terrainSamples) + col]);
            smp.height = height;
            smp.materialIndex0 = 0;
            smp.materialIndex1 = 0;
        }
    }

    heightFieldInfo.hfDesc.samples.data = samples;
    heightFieldInfo.hfDesc.samples.stride = sizeof(physx::PxU32);
    heightFieldInfo.colScale = 600 / (terrainSamples - 1);
    heightFieldInfo.rowScale = 600 / (terrainSamples - 1);
    heightFieldInfo.heightScale = 1;
    heightFieldComp->init(&heightFieldInfo);

    transformComp->event_visited.subscribe([this](scene::TreeTraverser* traverser) {
        meshComp->visit(traverser);
        heightFieldComp->visit(traverser);
    });
}
