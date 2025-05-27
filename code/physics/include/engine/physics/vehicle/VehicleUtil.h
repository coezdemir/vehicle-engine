//
// Created by Cem on 27.02.2022.
//
#pragma once

#include "PxPhysXConfig.h"
#include "PxPhysicsAPI.h"

namespace engine::physics::vehicle
{
    using namespace physx;

    //Drivable surface types.
    enum SurfaceType
    {
        SURFACE_TYPE_TARMAC = 0,
        MAX_NUM_SURFACE_TYPES
    };

    //Tire types.
    enum TireType
    {
        TIRE_TYPE_NORMAL = 0,
        MAX_NUM_TIRE_TYPES
    };

    enum
    {
        DRIVABLE_SURFACE = 0xffff0000,
        UNDRIVABLE_SURFACE = 0x0000ffff
    };

    enum
    {
        COLLISION_FLAG_GROUND = 1 << 0,
        COLLISION_FLAG_WHEEL = 1 << 1,
        COLLISION_FLAG_CHASSIS = 1 << 2,
        COLLISION_FLAG_OBSTACLE = 1 << 3,
        COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,

        COLLISION_FLAG_GROUND_AGAINST =
        COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
        COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
        COLLISION_FLAG_CHASSIS_AGAINST =
        COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE |
        COLLISION_FLAG_DRIVABLE_OBSTACLE,
        COLLISION_FLAG_OBSTACLE_AGAINST =
        COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE |
        COLLISION_FLAG_DRIVABLE_OBSTACLE,
        COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST =
        COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE
    };

    struct ActorUserData
    {
        ActorUserData()
                : vehicle(nullptr),
                  actor(nullptr)
        {
        }

        const PxVehicleWheels* vehicle;
        const PxActor* actor;
    };

    struct ShapeUserData
    {
        ShapeUserData()
                : isWheel(false),
                  wheelId(0xffffffff)
        {
        }

        bool isWheel;
        PxU32 wheelId;
    };

    enum DriveMode
    {
        eDRIVE_MODE_ACCEL_FORWARDS = 0,
        eDRIVE_MODE_ACCEL_REVERSE,
        eDRIVE_MODE_HARD_TURN_LEFT,
        eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
        eDRIVE_MODE_HARD_TURN_RIGHT,
        eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
        eDRIVE_MODE_BRAKE,
        eDRIVE_MODE_NONE
    };

    //Tire model friction for each combination of drivable surface type and tire type.
    static PxF32 getTireFrictionMultipliers[engine::physics::vehicle::SurfaceType::MAX_NUM_SURFACE_TYPES][engine::physics::vehicle::TireType::MAX_NUM_TIRE_TYPES] =
            {
                    //NORMAL
                    {1.00f} //TARMAC
            };

    struct VehicleDesc
    {
        VehicleDesc()
                : chassisMass(0.0f),
                  chassisDims(PxVec3(0.0f, 0.0f, 0.0f)),
                  chassisMOI(PxVec3(0.0f, 0.0f, 0.0f)),
                  chassisCMOffset(PxVec3(0.0f, 0.0f, 0.0f)),
                  chassisMaterial(nullptr),
                  wheelMass(0.0f),
                  wheelWidth(0.0f),
                  wheelRadius(0.0f),
                  wheelMOI(0.0f),
                  wheelMaterial(nullptr),
                  actorUserData(nullptr),
                  shapeUserDatas(nullptr),
                  numWheels(0)
        {
        }

        PxF32 chassisMass;
        PxVec3 chassisDims;
        PxVec3 chassisMOI;
        PxVec3 chassisCMOffset;
        PxMaterial* chassisMaterial;
        PxFilterData chassisSimFilterData;  //word0 = collide type, word1 = collide against types, word2 = PxPairFlags
        PxF32 wheelMass;
        PxF32 wheelWidth;
        PxF32 wheelRadius;
        PxF32 wheelMOI;
        PxMaterial* wheelMaterial;
        PxU32 numWheels;
        PxFilterData wheelSimFilterData;    //word0 = collide type, word1 = collide against types, word2 = PxPairFlags
        ActorUserData* actorUserData;
        ShapeUserData* shapeUserDatas;
    };

    // Data structure for quick setup of scene queries for suspension queries.
    struct VehicleSceneQueryData
    {
    public:
        VehicleSceneQueryData() :
                mNumQueriesPerBatch(0),
                mNumHitResultsPerQuery(0),
                mRaycastResults(nullptr),
                mRaycastHitBuffer(nullptr),
                mPreFilterShader(nullptr),
                mPostFilterShader(nullptr)
        {

        }

        ~VehicleSceneQueryData()
        {

        }

        /// \brief Allocate scene query data for up to maxNumVehicles and up to maxNumWheelsPerVehicle with numVehiclesInBatch per batch query.
        static VehicleSceneQueryData* allocate
                (const PxU32 maxNumVehicles, const PxU32 maxNumWheelsPerVehicle, const PxU32 maxNumHitPointsPerWheel,
                 const PxU32 numVehiclesInBatch, PxBatchQueryPreFilterShader preFilterShader,
                 PxBatchQueryPostFilterShader postFilterShader, PxAllocatorCallback& allocator)
        {
            const PxU32 sqDataSize = ((sizeof(VehicleSceneQueryData) + 15) & ~15);

            const PxU32 maxNumWheels = maxNumVehicles * maxNumWheelsPerVehicle;
            const PxU32 raycastResultSize = ((sizeof(PxRaycastQueryResult) * maxNumWheels + 15) & ~15);
            const PxU32 sweepResultSize = ((sizeof(PxSweepQueryResult) * maxNumWheels + 15) & ~15);

            const PxU32 maxNumHitPoints = maxNumWheels * maxNumHitPointsPerWheel;
            const PxU32 raycastHitSize = ((sizeof(PxRaycastHit) * maxNumHitPoints + 15) & ~15);
            const PxU32 sweepHitSize = ((sizeof(PxSweepHit) * maxNumHitPoints + 15) & ~15);

            const PxU32 size = sqDataSize + raycastResultSize + raycastHitSize + sweepResultSize + sweepHitSize;
            PxU8* buffer = static_cast<PxU8*>(allocator.allocate(size, NULL, NULL, 0));

            auto* sqData = new(buffer) VehicleSceneQueryData();
            sqData->mNumQueriesPerBatch = numVehiclesInBatch * maxNumWheelsPerVehicle;
            sqData->mNumHitResultsPerQuery = maxNumHitPointsPerWheel;
            buffer += sqDataSize;

            sqData->mRaycastResults = reinterpret_cast<PxRaycastQueryResult*>(buffer);
            buffer += raycastResultSize;

            sqData->mRaycastHitBuffer = reinterpret_cast<PxRaycastHit*>(buffer);
            buffer += raycastHitSize;

            sqData->mSweepResults = reinterpret_cast<PxSweepQueryResult*>(buffer);
            buffer += sweepResultSize;

            sqData->mSweepHitBuffer = reinterpret_cast<PxSweepHit*>(buffer);
            buffer += sweepHitSize;

            for (PxU32 i = 0; i < maxNumWheels; i++)
            {
                new(sqData->mRaycastResults + i) PxRaycastQueryResult();
                new(sqData->mSweepResults + i) PxSweepQueryResult();
            }

            for (PxU32 i = 0; i < maxNumHitPoints; i++)
            {
                new(sqData->mRaycastHitBuffer + i) PxRaycastHit();
                new(sqData->mSweepHitBuffer + i) PxSweepHit();
            }

            sqData->mPreFilterShader = preFilterShader;
            sqData->mPostFilterShader = postFilterShader;

            return sqData;
        }

        // Free allocated buffers.
        void free(PxAllocatorCallback& allocator)
        {
            allocator.deallocate(this);
        }

        /// \brief Create a PxBatchQuery instance that will be used for a single specified batch.
        static PxBatchQuery*
        setUpBatchedSceneQuery(const PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, PxScene* scene)
        {
            const PxU32 maxNumQueriesInBatch = vehicleSceneQueryData.mNumQueriesPerBatch;
            const PxU32 maxNumHitResultsInBatch =
                    vehicleSceneQueryData.mNumQueriesPerBatch * vehicleSceneQueryData.mNumHitResultsPerQuery;

            PxBatchQueryDesc sqDesc(maxNumQueriesInBatch, maxNumQueriesInBatch, 0);

            sqDesc.queryMemory.userRaycastResultBuffer =
                    vehicleSceneQueryData.mRaycastResults + batchId * maxNumQueriesInBatch;
            sqDesc.queryMemory.userRaycastTouchBuffer =
                    vehicleSceneQueryData.mRaycastHitBuffer + batchId * maxNumHitResultsInBatch;
            sqDesc.queryMemory.raycastTouchBufferSize = maxNumHitResultsInBatch;

            sqDesc.queryMemory.userSweepResultBuffer =
                    vehicleSceneQueryData.mSweepResults + batchId * maxNumQueriesInBatch;
            sqDesc.queryMemory.userSweepTouchBuffer =
                    vehicleSceneQueryData.mSweepHitBuffer + batchId * maxNumHitResultsInBatch;
            sqDesc.queryMemory.sweepTouchBufferSize = maxNumHitResultsInBatch;

            sqDesc.preFilterShader = vehicleSceneQueryData.mPreFilterShader;

            sqDesc.postFilterShader = vehicleSceneQueryData.mPostFilterShader;

            return scene->createBatchQuery(sqDesc);
        }

        // Return an array of scene query results for a single specified batch.
        PxRaycastQueryResult* getRaycastQueryResultBuffer(const PxU32 batchId)
        {
            return (mRaycastResults + batchId * mNumQueriesPerBatch);
        }

        // Return an array of scene query results for a single specified batch.
        PxSweepQueryResult* getSweepQueryResultBuffer(const PxU32 batchId)
        {
            return (mSweepResults + batchId * mNumQueriesPerBatch);
        }

        /// \brief Get the number of scene query results that have been allocated for a single batch.
        PxU32 getQueryResultBufferSize() const
        {
            return mNumQueriesPerBatch;
        }

    private:
        // Number of queries per batch
        PxU32 mNumQueriesPerBatch;

        // Number of hit results per query
        PxU32 mNumHitResultsPerQuery;

        // One result for each wheel.
        PxRaycastQueryResult* mRaycastResults;
        PxSweepQueryResult* mSweepResults{};

        // One hit for each wheel.
        PxRaycastHit* mRaycastHitBuffer;
        PxSweepHit* mSweepHitBuffer{};

        // Filter shader used to filter drivable and non-drivable surfaces
        PxBatchQueryPreFilterShader mPreFilterShader;

        // Filter shader used to reject hit shapes that initially overlap sweeps.
        PxBatchQueryPostFilterShader mPostFilterShader;
    };

    struct DebugRaycastVehicle
    {
        float chassisDims[3] = {0.f, 0.f, 0.f};
        float chassisMass;
        float suspensionLength;
        float suspensionStrength;
        float suspensionDampening;
        float centerOfMassOffset[3] = {0.f, 0.f, 0.f};
        float force = 50.f;
        float accelerationFactor = 100.f;
        float turningRate = 0.1f;
        float friction = 1.f;
        float forwardSpeed = 0.f;
        float linearDamping;
        float angularDamping;
    };

    struct DebugRaycastVehicle4W
    {
        /* Vehicle Description */
        float chassisMass;
        float chassisDims[3] = {0.f, 0.f, 0.f};
        float chassisMOI[3] = {0.f, 0.f, 0.f};
        float chassisCMOffset[3] = {0.f, 0.f, 0.f};
        float wheelMass;
        float wheelRadius;
        float wheelWidth;
        float wheelMOI;

        /* Suspension */
        float maxCompression;
        float maxDroop;
        float springStrength;
        float springDamperRate;
        float sprungMass;
        float camberAtRest;
        float camberAtMaxDroop;
        float camberAtMaxCompression;

        /* Drive Sim Data */
        float enginPeakTorq;
        float maxOmega;
        float gearSwitchTime;
        float clutchStrength;
        float ackermannAcuracy;
        float ackermannFrontWidth;
        float ackermannRearWidth;
        float ackermannAxleSeparation;

        /* Physics Materials */
        float drivableMat[3] = {0.f, 0.f, 0.f};
        float nonDrivableMat[3] = {0.f, 0.f, 0.f};

        /* Drive Info */
        float forwardSpeed;
        float forwardSpeedAbs;
        float sidewaysSpeed;
        float sidewaysSpeedAbs;
        int currentGear;
        int targetGear;
        bool bIsInReverse;

        /* Keyboard Input */
        bool bIsAccelerating;
        bool bIsBraking;
    };

    struct DebugVehicleTank
    {
        /* Vehicle Description */
        float chassisMass;
        float chassisDims[3] = {0.f, 0.f, 0.f};
        float chassisMOI[3] = {0.f, 0.f, 0.f};
        float chassisCMOffset[3] = {0.f, 0.f, 0.f};
        float wheelMass;
        float wheelRadius;
        float wheelWidth;
        float wheelMOI;

        /* Suspension */
        float maxCompression;
        float maxDroop;
        float springStrength;
        float springDamperRate;
        float sprungMass;

        /* Drive Sim Data */
        float enginPeakTorq;
        float maxOmega;
        float gearSwitchTime;
        float clutchStrength;
        float ackermannAcuracy;
        float ackermannFrontWidth;
        float ackermannRearWidth;
        float ackermannAxleSeparation;

        /* Physics Materials */
        float drivableMat[3] = {0.f, 0.f, 0.f};
        float nonDrivableMat[3] = {0.f, 0.f, 0.f};

        /* Drive Info */
        float forwardSpeed;
        float forwardSpeedAbs;
        float sidewaysSpeed;
        float sidewaysSpeedAbs;
        int currentGear;
        int targetGear;
        bool bIsInReverse;

        /* Keyboard Input */
        bool bIsAccelerating;
        bool bThrustLeft;
        bool bThrustRight;
        bool bBreakLeft;
        bool bBreakRight;
        bool bIsBraking;
    };

    struct VehicleKeyInput
    {
        VehicleKeyInput()
        {
            isKeyWPressedDown = false;
            isKeySPressedDown = false;
            isKeyAPressedDown = false;
            isKeyDPressedDown = false;
        }

        bool isKeyWPressedDown;
        bool isKeySPressedDown;
        bool isKeyAPressedDown;
        bool isKeyDPressedDown;
    };

    struct VehicleControllerInput
    {
        VehicleControllerInput()
        {
            accelerationValue = 0.f;
            breakValue = 0.f;
            steeringValue = 0.f;
        }

        float accelerationValue;
        float breakValue;
        float steeringValue;
    };
}