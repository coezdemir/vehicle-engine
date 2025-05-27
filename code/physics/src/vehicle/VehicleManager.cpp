//
// Created by Cem on 06.04.2022.
//
#include "engine/physics/vehicle/VehicleManager.h"
#include "engine/physics/Physics.h"

using namespace physx;

PxQueryHitType::Enum WheelSceneQueryPreFilterBlocking
        (PxFilterData filterData0, PxFilterData filterData1,
         const void* constantBlock, PxU32 constantBlockSize,
         PxHitFlags& queryFlags)
{
    //filterData0 is the vehicle suspension query.
    //filterData1 is the shape potentially hit by the query.
    PX_UNUSED(filterData0);
    PX_UNUSED(constantBlock);
    PX_UNUSED(constantBlockSize);
    PX_UNUSED(queryFlags);
    return ((0 == (filterData1.word3 & engine::physics::vehicle::DRIVABLE_SURFACE)) ? PxQueryHitType::eNONE : PxQueryHitType::eBLOCK);
}

PxQueryHitType::Enum WheelSceneQueryPostFilterBlocking
        (PxFilterData filterData0, PxFilterData filterData1,
         const void* constantBlock, PxU32 constantBlockSize,
         const PxQueryHit& hit)
{
    PX_UNUSED(filterData0);
    PX_UNUSED(filterData1);
    PX_UNUSED(constantBlock);
    PX_UNUSED(constantBlockSize);
    if((static_cast<const PxSweepHit&>(hit)).hadInitialOverlap())
        return PxQueryHitType::eNONE;
    return PxQueryHitType::eBLOCK;
}

PxQueryHitType::Enum WheelSceneQueryPreFilterNonBlocking
        (PxFilterData filterData0, PxFilterData filterData1,
         const void* constantBlock, PxU32 constantBlockSize,
         PxHitFlags& queryFlags)
{
    //filterData0 is the vehicle suspension query.
    //filterData1 is the shape potentially hit by the query.
    PX_UNUSED(filterData0);
    PX_UNUSED(constantBlock);
    PX_UNUSED(constantBlockSize);
    PX_UNUSED(queryFlags);
    return ((0 == (filterData1.word3 & engine::physics::vehicle::DRIVABLE_SURFACE)) ? PxQueryHitType::eNONE : PxQueryHitType::eTOUCH);
}

PxQueryHitType::Enum WheelSceneQueryPostFilterNonBlocking
        (PxFilterData filterData0, PxFilterData filterData1,
         const void* constantBlock, PxU32 constantBlockSize,
         const PxQueryHit& hit)
{
    PX_UNUSED(filterData0);
    PX_UNUSED(filterData1);
    PX_UNUSED(constantBlock);
    PX_UNUSED(constantBlockSize);
    if ((static_cast<const PxSweepHit&>(hit)).hadInitialOverlap())
        return PxQueryHitType::eNONE;
    return PxQueryHitType::eTOUCH;
}

engine::physics::vehicle::VehicleManager::VehicleManager(engine::physics::PhysicsSystem* physicsSystem) :
    physicsSystem(physicsSystem), bAutomaticInit(bAutomaticInit)
{
    pxScene = physicsSystem->getPxScene();
    pxAllocator = physicsSystem->getPxAllocator();
    vehicleSceneQueryData[FOUR_WHEELED_DATA_INDEX] = nullptr;
    vehicleSceneQueryData[TANK_DATA_INDEX] = nullptr;
    batchQuery[FOUR_WHEELED_DATA_INDEX] = nullptr;
    batchQuery[TANK_DATA_INDEX] = nullptr;
    numberOfVehicles[FOUR_WHEELED_DATA_INDEX] = 0;
    numberOfVehicles[TANK_DATA_INDEX] = 0;
    bIsInitialized = false;
}

engine::physics::vehicle::VehicleManager::~VehicleManager()
{
    for (int i = 0; i < VEHICLE_TYPES; ++i)
    {
        if(vehicleSceneQueryData[i])
            vehicleSceneQueryData[i]->free(*pxAllocator);

        if(frictionPairs[i])
            frictionPairs[i]->release();
    }
}

void engine::physics::vehicle::VehicleManager::registerVehicle(physx::PxVehicleDrive** vehicle)
{
    if((*vehicle)->mWheelsSimData.getNbWheelData() == 4)
        fourwheeledVehicle[numberOfVehicles[FOUR_WHEELED_DATA_INDEX]++] = vehicle;
    else
        tankVehicles[numberOfVehicles[TANK_DATA_INDEX]++] = vehicle;
}

void engine::physics::vehicle::VehicleManager::initVehicleSimulation(int dataIndex)
{
    createSceneQueryData();

    frictionPairs[dataIndex] = createFrictionPairs();

    bIsInitialized = true;
}

void engine::physics::vehicle::VehicleManager::createSceneQueryData()
{
    for (int i = 0; i < VEHICLE_TYPES; ++i)
    {
        PxU32 numberOfWheels = i == FOUR_WHEELED_DATA_INDEX ? 4 : 14;

        vehicleSceneQueryData[i] =
                engine::physics::vehicle::VehicleSceneQueryData::allocate(numberOfVehicles[i],
                                                                          numberOfVehicles[i] * numberOfWheels,
                                                                          1, numberOfVehicles[i],
                                                                          WheelSceneQueryPreFilterBlocking,
                                                                          nullptr,
                                                                          *pxAllocator);

        batchQuery[i] =
                engine::physics::vehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(
                        BATCH_ID[i], *vehicleSceneQueryData[i], pxScene);
    }
}

PxVehicleDrivableSurfaceToTireFrictionPairs* engine::physics::vehicle::VehicleManager::createFrictionPairs()
{
    PxVehicleDrivableSurfaceType surfaceTypes[1];
    surfaceTypes[0].mType = engine::physics::vehicle::SurfaceType::SURFACE_TYPE_TARMAC;

    const PxMaterial* surfaceMaterials[1];
    surfaceMaterials[0] = physicsSystem->getDefaultMaterial();

    PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
            PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(
                    engine::physics::vehicle::TireType::MAX_NUM_TIRE_TYPES,
                    engine::physics::vehicle::SurfaceType::MAX_NUM_SURFACE_TYPES
            );

    surfaceTirePairs->setup(engine::physics::vehicle::TireType::MAX_NUM_TIRE_TYPES,
                            engine::physics::vehicle::SurfaceType::MAX_NUM_SURFACE_TYPES,
                            surfaceMaterials, surfaceTypes);

    for (PxU32 i = 0; i < engine::physics::vehicle::SurfaceType::MAX_NUM_SURFACE_TYPES; ++i)
    {
        for (PxU32 j = 0; j < engine::physics::vehicle::TireType::MAX_NUM_TIRE_TYPES; ++j)
        {
            surfaceTirePairs->setTypePairFriction(i, j,
                                                  engine::physics::vehicle::getTireFrictionMultipliers[i][j]);
        }
    }
    return surfaceTirePairs;
}

void engine::physics::vehicle::VehicleManager::stepVehiclePhysics(const float timestep)
{
    if(!bIsInitialized)
    {
        if(fourwheeledVehicle[0])
            initVehicleSimulation(0);

        if(tankVehicles[0])
            initVehicleSimulation(1);
    }

    for (int i = 0; i < VEHICLE_TYPES; ++i)
    {
        if(!numberOfVehicles[i])
            continue;

        PxU32 numberOfWheels = i == FOUR_WHEELED_DATA_INDEX ? 4 : 14;

        PxVehicleWheels* vehicles[MAX_NUM_VEHICLES_PER_TYPE] = {nullptr, nullptr, nullptr, nullptr};
        for (int j = 0; j < numberOfVehicles[i]; ++j)
        {
            if(i == 0)
                vehicles[j] = *fourwheeledVehicle[j];
            else
                vehicles[j] = *tankVehicles[j];
        }


        PxVehicleSuspensionRaycasts(batchQuery[i], numberOfVehicles[i], vehicles,
                                    vehicleSceneQueryData[i]->getQueryResultBufferSize(),
                                    vehicleSceneQueryData[i]->getRaycastQueryResultBuffer(BATCH_ID[i]));

        //Vehicle update.
        const PxVec3 grav = pxScene->getGravity();
        PxWheelQueryResult wheelQueryResults[MAX_NUM_WHEELS];

        PxVehicleWheelQueryResult vehicleQueryResults[MAX_NUM_VEHICLES_PER_TYPE] = {{wheelQueryResults, numberOfWheels},
                                                                                    {wheelQueryResults, numberOfWheels},
                                                                                    {wheelQueryResults, numberOfWheels},
                                                                                    {wheelQueryResults, numberOfWheels}};
        PxVehicleUpdates(timestep, grav, *frictionPairs[i], numberOfVehicles[i],
                         vehicles, vehicleQueryResults);
    }

 }

int engine::physics::vehicle::VehicleManager::getNbOfRegisteredVehicles(const int vehicleType) const
{
    return numberOfVehicles[vehicleType];
}

bool engine::physics::vehicle::VehicleManager::isInitialized() const
{
    return bIsInitialized;
}