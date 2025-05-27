//
// Created by Cem on 26.06.2022.
//
#include "engine/physics/components/collision/CHeightField.h"

engine::physics::components::CHeightField::CHeightField(engine::components::id_t id, engine::entities::id_t owner)
        : CCollision(id, owner)
{

}

void engine::physics::components::CHeightField::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CCollision::init(constructionInfo);

    // Check if we have the right construction info
    auto* heightFieldConstruction =
            dynamic_cast<const engine::physics::components::CHeightField::ConstructionInfo*>(constructionInfo);
    assert(heightFieldConstruction && "Please use the correct ConstructionInfo: CHeightField::ConstructionInfo");

    hfDesc = heightFieldConstruction->hfDesc;
    rowScale = heightFieldConstruction->rowScale;
    colScale = heightFieldConstruction->colScale;
    heightScale = heightFieldConstruction->heightScale;
}

void engine::physics::components::CHeightField::initPhysics(const glm::mat4& initTransform)
{
    CPhysicsComponent::initPhysics(initTransform);

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    decomposeTransform(initTransform, translation, rotation, scale);

    const physx::PxShapeFlags shapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE
                                            | physx::PxShapeFlag::eSIMULATION_SHAPE;

    heightField = pxCooking->createHeightField(hfDesc, pxPhysics->getPhysicsInsertionCallback());

    rigidBody = pxPhysics->createRigidStatic(glmToPhysx(transform));

    physx::PxHeightFieldGeometry hfGeom = physx::PxHeightFieldGeometry(heightField, physx::PxMeshGeometryFlags(), heightScale,
                                        rowScale, colScale);
    shape = physx::PxRigidActorExt::createExclusiveShape(*rigidBody, hfGeom, *pxDefaultMat, shapeFlags);
    shape->setSimulationFilterData(simulationFilterData);
    shape->setQueryFilterData(qryFilterData);

    fireRigidActorEvent(&event_onRigidBodyCreated);
}

void engine::physics::components::CHeightField::destroyPhysics()
{
    delete [] (physx::PxU32*) hfDesc.samples.data;
    CPhysicsComponent::destroyPhysics();
}
