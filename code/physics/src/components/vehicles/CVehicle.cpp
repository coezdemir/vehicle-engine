//
// Created by Cem on 26.05.2022.
//

#include "engine/physics/components/vehicles/CVehicle.h"
#include "engine/physics/Physics.h"

engine::physics::components::CVehicle::CVehicle(engine::components::id_t id, entities::id_t owner)
        : CPhysicsComponent(id, owner)
{
    bIsActive = false;
    bIsControlledByController = false;
    bIsInReverse = false;
    bIsMovingForwardSlowly = false;
    rigidBody = nullptr;
    shape = nullptr;
    bNeedsUpdate = false;
}

void engine::physics::components::CVehicle::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* const raycastVehicleConstruction =
            dynamic_cast<const engine::physics::components::CVehicle::ConstructionInfo*>(constructionInfo);
    assert(raycastVehicleConstruction &&
                   "Please use the correct ConstructionInfo: VehicleComponent::ConstructionInfo");

    vehicleDesc = raycastVehicleConstruction->vehicleDesc;
    vehicleDesc.chassisMaterial =
            !vehicleDesc.chassisMaterial ? physicsSystem->getDefaultMaterial() : vehicleDesc.chassisMaterial;
    vehicleDesc.wheelMaterial =
            !vehicleDesc.wheelMaterial ? physicsSystem->getDefaultMaterial() : vehicleDesc.wheelMaterial;
}

void engine::physics::components::CVehicle::updateTransform()
{
    if (bIsActive && vehicle)
    {
        physx::PxTransform newTransform = vehicle->getRigidDynamicActor()->getGlobalPose();
        physx::PxMat44 mat44 = physx::PxMat44(newTransform);

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
                transform[i][j] = mat44[i][j];
        }

        bNeedsUpdate = true;
    }
}

void engine::physics::components::CVehicle::initialVisit(scene::TreeTraverser* traverser)
{
    bInitialTraversed = true;

    transform = traverser->getStackTop();
    createVehiclePhysics();
}

void engine::physics::components::CVehicle::visit(scene::TreeTraverser* traverser)
{
    if (!bInitialTraversed)
    {
        initialVisit(traverser);
        return;
    }

    engine::scene::components::transform_handle_t transformComponent = traverser->getCurrTransformComp();

    if (!bNeedsUpdate || !bIsActive)
        return;

    glm::mat4 oldLocal = traverser->popStack();

    transformComponent->setTransform(transform);

    glm::mat4 newLocal = glm::inverse(traverser->getStackTop()) * transform;
    transformComponent->setLocalTransform(newLocal);

    traverser->pushStack(traverser->getStackTop() * transformComponent->getLocalTransform());

    bNeedsUpdate = true;
}

physx::PxVehicleDrivableSurfaceToTireFrictionPairs* engine::physics::components::CVehicle::createFrictionPairs()
{
    physx::PxVehicleDrivableSurfaceType surfaceTypes[1];
    surfaceTypes[0].mType = engine::physics::vehicle::SurfaceType::SURFACE_TYPE_TARMAC;

    const physx::PxMaterial* surfaceMaterials[1];
    surfaceMaterials[0] = physicsSystem->getDefaultMaterial();

    physx::PxVehicleDrivableSurfaceToTireFrictionPairs* newPair = physx::PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(
            engine::physics::vehicle::TireType::MAX_NUM_TIRE_TYPES,
            engine::physics::vehicle::SurfaceType::MAX_NUM_SURFACE_TYPES
    );

    newPair->setup(engine::physics::vehicle::TireType::MAX_NUM_TIRE_TYPES,
                   engine::physics::vehicle::SurfaceType::MAX_NUM_SURFACE_TYPES,
                   surfaceMaterials, surfaceTypes);

    for (physx::PxU32 i = 0; i < engine::physics::vehicle::SurfaceType::MAX_NUM_SURFACE_TYPES; i++)
    {
        for (physx::PxU32 j = 0; j < engine::physics::vehicle::TireType::MAX_NUM_TIRE_TYPES; j++)
        {
            newPair->setTypePairFriction(i, j,
                                         engine::physics::vehicle::getTireFrictionMultipliers[i][j]);
        }
    }

    return newPair;
}

void engine::physics::components::CVehicle::destroyPhysics()
{
    if (bIsActive)
    {
        deactivate();
        vehicle->release();
        vehicle = nullptr;
        bPhysicsInitialized = false;
    }
}

physx::PxConvexMesh*
engine::physics::components::CVehicle::createWheelMesh(const float width, const float radius)
{
    physx::PxVec3 points[2 * 16];
    for (physx::PxU32 i = 0; i < 16; ++i)
    {
        const physx::PxF32 cosTheta = physx::PxCos(i * physx::PxPi * 2.0f / 16.0f);
        const physx::PxF32 sinTheta = physx::PxSin(i * physx::PxPi * 2.0f / 16.0f);
        const physx::PxF32 y = radius * cosTheta;
        const physx::PxF32 z = radius * sinTheta;
        points[2 * i + 0] = physx::PxVec3(-width / 2.0f, y, z);
        points[2 * i + 1] = physx::PxVec3(+width / 2.0f, y, z);
    }

    return physicsSystem->createConvexMesh(points, 32);
}

physx::PxConvexMesh* engine::physics::components::CVehicle::createChassisMesh(const physx::PxVec3& dims)
{
    const physx::PxF32 x = dims.x * 0.5f;
    const physx::PxF32 y = dims.y * 0.5f;
    const physx::PxF32 z = dims.z * 0.5f;
    physx::PxVec3 verts[8] =
            {
                    physx::PxVec3(x, y, -z),
                    physx::PxVec3(x, y, z),
                    physx::PxVec3(x, -y, z),
                    physx::PxVec3(x, -y, -z),
                    physx::PxVec3(-x, y, -z),
                    physx::PxVec3(-x, y, z),
                    physx::PxVec3(-x, -y, z),
                    physx::PxVec3(-x, -y, -z)
            };

    return physicsSystem->createConvexMesh(verts, 8);
}

physx::PxRigidDynamic*
engine::physics::components::CVehicle::createVehicleActor(const physx::PxVehicleChassisData& chassisData,
                                                          physx::PxMaterial** wheelMaterials,
                                                          physx::PxConvexMesh** wheelMeshes,
                                                          const unsigned int numWheels,
                                                          const physx::PxFilterData& wheelSimFilterData,
                                                          const physx::PxMaterial** chassisMaterials,
                                                          physx::PxConvexMesh** chassisMeshes,
                                                          const int numChassisMeshes,
                                                          const physx::PxFilterData& chassisSimFilterData)
{
    //We need a rigid body actor for the vehicle.
    //Don't forget to add the actor to the scene after setting up the associated vehicle.
    physx::PxRigidDynamic* vehActor = pxPhysics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));
    vehActor->userData = this;

    //Wheel and chassis query filter data.
    //Optional: cars don't drive on other cars.
    physx::PxFilterData wheelQryFilterData;
    wheelQryFilterData.word3 = physics::vehicle::UNDRIVABLE_SURFACE;
    physx::PxFilterData chassisQryFilterData;
    chassisQryFilterData.word3 = physics::vehicle::UNDRIVABLE_SURFACE;

    //Add all the wheel shapes to the actor.
    for (physx::PxU32 i = 0; i < numWheels; i++)
    {
        physx::PxConvexMeshGeometry geom(wheelMeshes[i]);
        physx::PxShape* wheelShape = physx::PxRigidActorExt::createExclusiveShape(*vehActor, geom, *wheelMaterials[i]);
        wheelShape->setQueryFilterData(wheelQryFilterData);
        wheelShape->setSimulationFilterData(vehicleDesc.wheelSimFilterData);
        wheelShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
    }

    //Add the chassis shapes to the actor.
    for (physx::PxU32 i = 0; i < numChassisMeshes; i++)
    {
        physx::PxShape* chassisShape = physx::PxRigidActorExt::createExclusiveShape(*vehActor,
                                                                                    physx::PxConvexMeshGeometry(
                                                                                            chassisMeshes[i]),
                                                                                    *chassisMaterials[i]);
        chassisShape->setQueryFilterData(chassisQryFilterData);
        chassisShape->setSimulationFilterData(vehicleDesc.chassisSimFilterData);
        chassisShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
    }

    vehActor->setMass(chassisData.mMass);
    vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
    vehActor->setCMassLocalPose(physx::PxTransform(chassisData.mCMOffset, physx::PxQuat(physx::PxIdentity)));

    return vehActor;
}

void
engine::physics::components::CVehicle::configureUserData()
{
    if (vehicleDesc.actorUserData)
    {
        vehicle->getRigidDynamicActor()->userData = vehicleDesc.actorUserData;
        vehicleDesc.actorUserData->vehicle = vehicle;
    }

    if (vehicleDesc.actorUserData)
    {
        physx::PxShape* shapes[PX_MAX_NB_WHEELS + 1];
        vehicle->getRigidDynamicActor()->getShapes(shapes, PX_MAX_NB_WHEELS + 1);
        for (physx::PxU32 i = 0; i < vehicle->mWheelsSimData.getNbWheels(); i++)
        {
            const physx::PxI32 shapeId = vehicle->mWheelsSimData.getWheelShapeMapping(i);
            shapes[shapeId]->userData = &vehicleDesc.shapeUserDatas[i];
            vehicleDesc.shapeUserDatas[i].isWheel = true;
            vehicleDesc.shapeUserDatas[i].wheelId = i;
        }
    }
}
