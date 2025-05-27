//
// Created by CemOe on 24.08.2021.
//

#include "engine/physics/components/collision/CComplexCollision.h"
#include "engine/components/ComponentPool.h"
#include "engine/physics/vehicle/VehicleUtil.h"

engine::physics::components::CComplexCollision::CComplexCollision(const engine::components::id_t id,
                                                                  const engine::entities::id_t owner)
        : CCollision(id, owner)
{

}

void
engine::physics::components::CComplexCollision::init(const engine::components::ConstructionInfo* const constructionInfo)
{
    CCollision::init(constructionInfo);

    auto* complexCollisionInfo =
            dynamic_cast<const engine::physics::components::CComplexCollision::ConstructionInfo*>(constructionInfo);
    assert(complexCollisionInfo &&
                   "Please use the correct ConstructionInfo: ComplexCollisionComponent::ConstructionInfo");

    vertices = complexCollisionInfo->vertices;
    indices = complexCollisionInfo->indices;
}

void engine::physics::components::CComplexCollision::initPhysics(const glm::mat4& transform)
{
    assert(pxCooking);

    CCollision::initPhysics(transform);

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    decomposeTransform(transform, translation, rotation, scale);

    physx::PxShapeFlags shapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE |
                                     physx::PxShapeFlag::eSIMULATION_SHAPE;

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

    for (int i = 0; i < vertices->size(); ++i)
    {
        std::vector<glm::vec3> v = (*vertices)[i];
        std::vector<unsigned int> ind = (*indices)[i];

        physx::PxTriangleMeshDesc meshDesc;
        meshDesc.setToDefault();
        meshDesc.points.count           = v.size();
        meshDesc.points.stride          = sizeof(glm::vec3);
        meshDesc.points.data            = &v[0];

        meshDesc.triangles.count        = v.size() / 3;
        meshDesc.triangles.stride       = 3 * sizeof(unsigned int);
        meshDesc.triangles.data         = &ind[0];

        if(!meshDesc.isValid())
            return;

        physx::PxMeshScale pxScale = physx::PxMeshScale(glmToPhysx(scale));

        physx::PxDefaultMemoryOutputStream writeBuffer;
        physx::PxTriangleMeshCookingResult::Enum result;
        bool status = pxCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);

        if(!status)
            return;

        triangleMesh = pxCooking->createTriangleMesh(meshDesc,pxPhysics->getPhysicsInsertionCallback());
        physx::PxTriangleMeshGeometry triangleMeshGeometry = physx::PxTriangleMeshGeometry(triangleMesh, pxScale);

        shape = pxPhysics->createShape(triangleMeshGeometry, *pxDefaultMat, true, shapeFlags);

        shape->setSimulationFilterData(simulationFilterData);
        shape->setQueryFilterData(qryFilterData);

        status = rigidBody->attachShape(*shape);
        shape->release();

        if(!status)
            shape = nullptr;
    }

    fireRigidActorEvent(&event_onRigidBodyCreated);
}