//
// Created by CemOe on 27.08.2021.
//

#include "engine/physics/components/collision/CBoxCollision.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "engine/physics/vehicle/VehicleUtil.h"

engine::physics::components::CBoxCollision::CBoxCollision(const engine::components::id_t id,
                                                          const entities::id_t owner)
        : CCollision(id, owner)
{

}

void engine::physics::components::CBoxCollision::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CCollision::init(constructionInfo);

    // Check if we have the right construction info
    auto* boxConstruction =
            dynamic_cast<const engine::physics::components::CBoxCollision::ConstructionInfo*>(constructionInfo);
    assert(boxConstruction && "Please use the correct ConstructionInfo: BoxCollisionComponent::ConstructionInfo");

    boxHalfExtent = boxConstruction->boxHalfExtent;
}

void engine::physics::components::CBoxCollision::initPhysics(const glm::mat4& initTransform)
{
    CCollision::initPhysics(initTransform);

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    decomposeTransform(initTransform, translation, rotation, scale);

    physx::PxVec3 pxBoxHalfExtents = glmToPhysx(boxHalfExtent);
    pxBoxHalfExtents.x *= scale.x;
    pxBoxHalfExtents.y *= scale.y;
    pxBoxHalfExtents.z *= scale.z;

    const physx::PxShapeFlags shapeFlags = bIsSimulationShape ?
            physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE :
            physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eTRIGGER_SHAPE;

    if (bIsDynamic)
    {
        physx::PxRigidDynamic* rigidDynamic = pxPhysics->createRigidDynamic(glmToPhysx(transform));
        rigidDynamic->setMass(mass);
        rigidBody = rigidDynamic;

        if(bIsKinematic)
            rigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
    }
    else
    {
        rigidBody = pxPhysics->createRigidStatic(glmToPhysx(transform));
        rigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
    }

    rigidBody->userData = this;

    shape = physx::PxRigidActorExt::createExclusiveShape(*rigidBody, physx::PxBoxGeometry(pxBoxHalfExtents), *pxDefaultMat,
                                                         shapeFlags);

    shape->setSimulationFilterData(simulationFilterData);
    shape->setQueryFilterData(qryFilterData);

    fireRigidActorEvent(&event_onRigidBodyCreated);
}