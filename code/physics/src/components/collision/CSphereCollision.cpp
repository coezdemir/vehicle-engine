//
// Created by CemOe on 27.08.2021.
//

#include "engine/scene/TreeTraverser.h"
#include "engine/physics/components/collision/CSphereCollision.h"
#include "glm/gtx/matrix_decompose.hpp"

engine::physics::components::CSphereCollision::CSphereCollision(const engine::components::id_t id,
                                                                const engine::entities::id_t owner)
                                                                : CCollision(id, owner)
{

}

void engine::physics::components::CSphereCollision::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CCollision::init(constructionInfo);

    // Check if we have the right construction info
    auto* sphereConstruction =
            dynamic_cast<const engine::physics::components::CSphereCollision::ConstructionInfo*>(constructionInfo);
    assert(sphereConstruction && "Please use the correct ConstructionInfo: SphereCollisionComponent::ConstructionInfo");

    sphereRadius = sphereConstruction->sphereRadius;
}

void engine::physics::components::CSphereCollision::initPhysics(const glm::mat4& transform)
{
    CCollision::initPhysics(transform);

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    glm::vec3 euler = glm::eulerAngles(rotation);

    physx::PxShapeFlags shapeFlags = bIsSimulationShape ?
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

    shape =
            physx::PxRigidActorExt::createExclusiveShape(*rigidBody,
                                                         physx::PxSphereGeometry(sphereRadius),
                                                         *pxDefaultMat,
                                                         shapeFlags);

    shape->setSimulationFilterData(simulationFilterData);
    shape->setQueryFilterData(qryFilterData);

    fireRigidActorEvent(&event_onRigidBodyCreated);
}