//
// Created by Cem on 29.05.2022.
//

#include "engine/physics/components/vehicles/CVehicleTank.h"
#include "engine/physics/Physics.h"

#define THRESHOLD_FORWARD_SPEED (0.1f)
#define THRESHOLD_SIDEWAYS_SPEED (0.2f)
#define THRESHOLD_ROLLING_BACKWARDS_SPEED (0.1f)
#define NUMBER_OF_WHEELS (8)

namespace engine::physics::vehicle
{
    PxVehicleKeySmoothingData keySmoothingDataTank =
    {
        {
                6.0f,   //rise rate eANALOG_INPUT_ACCEL
                6.0f,   //rise rate eANALOG_INPUT_BRAKE
                6.0f,   //rise rate eANALOG_INPUT_HANDBRAKE
                2.5f,   //rise rate eANALOG_INPUT_STEER_LEFT
                2.5f,   //rise rate eANALOG_INPUT_STEER_RIGHT
        },
        {
                10.0f,  //fall rate eANALOG_INPUT_ACCEL
                10.0f,  //fall rate eANALOG_INPUT_BRAKE
                10.0f,  //fall rate eANALOG_INPUT_HANDBRAKE
                5.0f,   //fall rate eANALOG_INPUT_STEER_LEFT
                5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
        }
    };

    PxVehiclePadSmoothingData padSmoothingDataTank =
    {
        {
                6.0f,   //rise rate eANALOG_INPUT_ACCEL
                6.0f,   //rise rate eANALOG_INPUT_BRAKE
                6.0f,   //rise rate eANALOG_INPUT_HANDBRAKE
                2.5f,   //rise rate eANALOG_INPUT_STEER_LEFT
                2.5f,   //rise rate eANALOG_INPUT_STEER_RIGHT
        },
        {
                10.0f,  //fall rate eANALOG_INPUT_ACCEL
                10.0f,  //fall rate eANALOG_INPUT_BRAKE
                10.0f,  //fall rate eANALOG_INPUT_HANDBRAKE
                5.0f,   //fall rate eANALOG_INPUT_STEER_LEFT
                5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
        }
    };

    PxF32 steerVsForwardSpeedDataTank[2 * 8] =
    {
            0.0f, 0.75f,
            5.0f, 0.75f,
            30.0f, 0.125f,
            120.0f, 0.1f,
            PX_MAX_F32, PX_MAX_F32,
            PX_MAX_F32, PX_MAX_F32,
            PX_MAX_F32, PX_MAX_F32,
            PX_MAX_F32, PX_MAX_F32
    };
}

engine::physics::components::CVehicleTank::CVehicleTank(const engine::components::id_t id,
                                                        const engine::entities::id_t owner)
                                                                        : CVehicle(id, owner),
                                                                          vehicleInputData(physx::PxVehicleDriveTankControlModel::eSTANDARD)
{
    bStartedLeftTurn = false;
    bStartedRightTurn = false;
    bBreakedLeft = false;
    bBreakedRight = false;
}

void engine::physics::components::CVehicleTank::createVehiclePhysics()
{
    if (pxScene && !bIsActive)
    {
        bIsActive = true;

        vehicleTank = createVehicle();
        vehicle = vehicleTank;

        //Configure the userdata
        configureUserData();

        transform = glm::translate(transform,
                                   glm::vec3(0, vehicleDesc.chassisDims.y * 0.25f + vehicleDesc.wheelRadius + 1.0f, 0.f));

        vehicleTank->getRigidDynamicActor()->setGlobalPose(glmToPhysx(transform));
        pxScene->addActor(*vehicleTank->getRigidDynamicActor());

        //Set the vehicle to rest in first gear.
        //Set the vehicle to use auto-gears.
        vehicleTank->setToRestState();
        vehicleTank->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
        vehicleTank->mDriveDynData.setUseAutoGears(true);
        vehicleTank->setDriveModel(physx::PxVehicleDriveTankControlModel::eSTANDARD);

        fireEvent(event_physicsInitialized, &vehicle);
    }
}

void engine::physics::components::CVehicleTank::tick(const float deltaTime)
{
    engine::physics::components::CVehicle::tick(deltaTime);

    physx::PxVehicleDriveDynData* driveDynData = &vehicleTank->mDriveDynData;
    physx::PxVehicleDriveTankRawInputData newInputData = physx::PxVehicleDriveTankRawInputData(physx::PxVehicleDriveTankControlModel::eSTANDARD);

    //Work out if the car is to flip from reverse to forward gear or from forward gear to reverse.
    bool bToggleAutoReverse = false;
    bool bNewIsMovingForwardSlowly = false;

    processAutoReverse(bToggleAutoReverse, bNewIsMovingForwardSlowly);

    bIsMovingForwardSlowly = bNewIsMovingForwardSlowly;

    //If the car is to flip gear direction then switch gear as appropriate.
    if(bToggleAutoReverse)
    {
        bIsInReverse = !bIsInReverse;

        if(bIsInReverse)
        {
            driveDynData->forceGearChange(physx::PxVehicleGearsData::eREVERSE);
        }
        else
        {
            driveDynData->forceGearChange(physx::PxVehicleGearsData::eFIRST);
        }
    }

    newInputData = vehicleInputData;

    if(bIsInReverse)
    {
        const bool accel = newInputData.getDigitalAccel();
        const bool brake = newInputData.getDigitalLeftBrake() && newInputData.getDigitalRightBrake();
        newInputData.setDigitalLeftBrake(accel);
        newInputData.setDigitalRightBrake(accel);
        newInputData.setDigitalAccel(brake);
        newInputData.setDigitalLeftThrust(newInputData.getDigitalLeftBrake());
        newInputData.setDigitalRightThrust(newInputData.getDigitalRightBrake());
    }

    if(!bIsControlledByController)
    {
        physx::PxVehicleDriveTankSmoothDigitalRawInputsAndSetAnalogInputs(engine::physics::vehicle::keySmoothingDataTank,
                                                                        newInputData, deltaTime, *vehicleTank);
    }
    else
    {
       physx::PxVehicleDriveTankSmoothAnalogRawInputsAndSetAnalogInputs(engine::physics::vehicle::padSmoothingDataTank,
                                                                       newInputData, deltaTime, *vehicleTank);
    }

    updateTransform();
}

physx::PxVehicleDriveTank* engine::physics::components::CVehicleTank::createVehicle()
{
    const physx::PxVec3 chassisDims = vehicleDesc.chassisDims;
    const physx::PxF32 wheelWidth = vehicleDesc.wheelWidth;
    const physx::PxF32 wheelRadius = vehicleDesc.wheelRadius;
    const physx::PxU32 numWheels = vehicleDesc.numWheels;

    const physx::PxFilterData& chassisSimFilterData = vehicleDesc.chassisSimFilterData;
    const physx::PxFilterData& wheelSimFilterData = vehicleDesc.wheelSimFilterData;

    //Construct a physx actor with shapes for the chassis and wheels.
    //Set the rigid body mass, moment of inertia, and center of mass offset.
    physx::PxRigidDynamic* tankActor = NULL;
    {
        //Construct a convex mesh for a cylindrical wheel.
        physx::PxConvexMesh* wheelMesh = createWheelMesh(wheelWidth, wheelRadius);
        //Assume all wheels are identical for simplicity.
        physx::PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
        physx::PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

        chassisMaterialDrivable = vehicleDesc.wheelMaterial ? vehicleDesc.wheelMaterial  : physicsSystem->getChassisMaterialDrivable();
        chassisMaterialNonDrivable = vehicleDesc.chassisMaterial ? vehicleDesc.chassisMaterial  : physicsSystem->getChassisMaterialNonDrivable();

        for(physx::PxU32 i = 0; i < numWheels; i++)
        {
            wheelConvexMeshes[i] = wheelMesh;
            wheelMaterials[i] = chassisMaterialDrivable;
        }

        //Chassis just has a single convex shape for simplicity.
        physx::PxConvexMesh* chassisConvexMesh = createChassisMesh(chassisDims);
        physx::PxConvexMesh* chassisConvexMeshes[1] = {chassisConvexMesh};
        const physx::PxMaterial* chassisMaterials[1] = {chassisMaterialNonDrivable};

        //Rigid body data.
        physx::PxVehicleChassisData rigidBodyData;
        rigidBodyData.mMOI = vehicleDesc.chassisMOI;
        rigidBodyData.mMass = vehicleDesc.chassisMass;
        rigidBodyData.mCMOffset = vehicleDesc.chassisCMOffset;

        tankActor = createVehicleActor
                (rigidBodyData,
                 wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData,
                 chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData);
    }

    //Set up the sim data for the wheels.
    physx::PxVehicleWheelsSimData* wheelsSimData = physx::PxVehicleWheelsSimData::allocate(numWheels);
    {
        //Compute the wheel center offsets from the origin.
        physx::PxVec3 wheelCentreActorOffsets[PX_MAX_NB_WHEELS];
        const physx::PxF32 frontZ = chassisDims.z * 0.35f;
        const physx::PxF32 rearZ = -chassisDims.z * 0.35f;
        CVehicleTank::computeWheelCenterActorOffsets(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels, wheelCentreActorOffsets);

        setupWheels
                (vehicleDesc.wheelMass, vehicleDesc.wheelMOI, wheelRadius, wheelWidth,
                 numWheels, wheelCentreActorOffsets,
                 vehicleDesc.chassisCMOffset, vehicleDesc.chassisMass,
                 wheelsSimData);
    }

    //Set up the sim data for the tank drive model.
    physx::PxVehicleDriveSimData driveSimData;
    {
        //Set up the engine to be more powerful but also more damped than the default engine.
        physx::PxVehicleEngineData engineData = driveSimData.getEngineData();
        engineData.mPeakTorque *= 3.f;
        engineData.mDampingRateZeroThrottleClutchEngaged = 2.f;
        engineData.mDampingRateZeroThrottleClutchDisengaged = 0.5f;
        engineData.mDampingRateFullThrottle = 0.5f;
        driveSimData.setEngineData(engineData);
    }

    //Create a tank from the wheels and drive sim data.
    physx::PxVehicleDriveTank* driveTank = physx::PxVehicleDriveTank::allocate(numWheels);
    driveTank->setup(pxPhysics, tankActor, *wheelsSimData, driveSimData, numWheels);

    //Free the sim data because we don't need that any more.
    wheelsSimData->free();

    return driveTank;
}

void
engine::physics::components::CVehicleTank::setupWheels(float wheelMass, float wheelMOI, const float wheelRadius,
                                                       const float wheelWidth, const unsigned int numWheels,
                                                       physx::PxVec3* wheelCenterActorOffsets,
                                                       physx::PxVec3 chassisCMOffset, float chassisMass,
                                                       physx::PxVehicleWheelsSimData* wheelsSimData)
{
    //Set up the wheels.
    physx::PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
    {
        //Set up the wheel data structures with mass, moi, radius, width.
        //Increase the damping on the wheel.
        for(physx::PxU32 i = 0; i < numWheels; i++)
        {
            wheels[i].mMass = wheelMass;
            wheels[i].mMOI = wheelMOI;
            wheels[i].mRadius = wheelRadius;
            wheels[i].mWidth = wheelWidth;
            wheels[i].mDampingRate = 2.0f;
        }
    }

    //Set up the tires.
    physx::PxVehicleTireData tires[PX_MAX_NB_WHEELS];
    {
        //Set all tire types to "normal" type.
        for(physx::PxU32 i = 0; i < numWheels; i++)
        {
            tires[i].mType = physics::vehicle::TireType::TIRE_TYPE_NORMAL;
        }
    }

    //Set up the suspensions
    physx::PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
    {
        //Compute the mass supported by each suspension spring.
        physx::PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
        PxVehicleComputeSprungMasses(numWheels, wheelCenterActorOffsets,
                                     chassisCMOffset, chassisMass, 1, suspSprungMasses);

        //Set the suspension data.
        for(physx::PxU32 i = 0; i < numWheels; i++)
        {
            suspensions[i].mMaxCompression = 0.3f;
            suspensions[i].mMaxDroop = 0.1f;
            suspensions[i].mSpringStrength = 10000.0f;
            suspensions[i].mSpringDamperRate = 1500.0f;
            suspensions[i].mSprungMass = suspSprungMasses[i];
        }
    }

    //Set up the wheel geometry.
    physx::PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
    physx::PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
    physx::PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
    physx::PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
    {
        for(physx::PxU32 i = 0; i < numWheels; i++)
        {
            //Vertical suspension travel.
            suspTravelDirections[i] = physx::PxVec3(0, -1, 0);

            //Wheel center offset is offset from rigid body center of mass.
            wheelCentreCMOffsets[i] = wheelCenterActorOffsets[i] - chassisCMOffset;

            //Suspension force application point 0.3 metres below rigid body center of mass.
            suspForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

            //Tire force application point 0.3 metres below rigid body center of mass.
            tireForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
        }
    }

    //Set up the filter data of the raycast that will be issued by each suspension.
    physx::PxFilterData qryFilterData;
    qryFilterData.word3 = physics::vehicle::DRIVABLE_SURFACE;

    //Set the wheel, tire and suspension data.
    //Set the geometry data.
    //Set the query filter data
    for(physx::PxU32 i = 0; i < numWheels; i++)
    {
        wheelsSimData->setWheelData(i, wheels[i]);
        wheelsSimData->setTireData(i, tires[i]);
        wheelsSimData->setSuspensionData(i, suspensions[i]);
        wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
        wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
        wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
        wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
        wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
        wheelsSimData->setWheelShapeMapping(i, physx::PxI32(i));
    }
}

void engine::physics::components::CVehicleTank::startBrakeMode()
{
    if(!bIsControlledByController)
    {
        if(!bStartedLeftTurn)
        {
            vehicleInputData.setDigitalLeftBrake(true);
            bBreakedLeft = true;
        }

        if(!bStartedRightTurn)
        {
            vehicleInputData.setDigitalRightBrake(true);
            bBreakedRight = true;
        }
    }
    else
    {
        if(!bStartedLeftTurn)
        {
            vehicleInputData.setAnalogLeftBrake(1.0f);
            bBreakedLeft = true;
        }

        if(!bStartedRightTurn)
        {
            vehicleInputData.setAnalogRightBrake(1.0f);
            bBreakedRight = true;
        }
    }
}

void engine::physics::components::CVehicleTank::processInput(const int key, const int action)
{
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        bStartedLeftTurn = true;
        vehicleInputData.setDigitalAccel(true);
        vehicleInputData.setDigitalRightThrust(true);
        vehicleInputData.setDigitalLeftBrake(true);
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        bStartedLeftTurn = false;
        vehicleInputData.setDigitalAccel(false);
        vehicleInputData.setDigitalRightThrust(false);
        vehicleInputData.setDigitalLeftBrake(false);
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        bStartedRightTurn = true;
        vehicleInputData.setDigitalAccel(true);
        vehicleInputData.setDigitalLeftThrust(true);
        vehicleInputData.setDigitalRightBrake(true);
    }
    else if(key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        bStartedRightTurn = false;
        vehicleInputData.setDigitalAccel(false);
        vehicleInputData.setDigitalLeftThrust(false);
        vehicleInputData.setDigitalRightBrake(false);
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        vehicleInputData.setDigitalAccel(true);
        vehicleInputData.setDigitalLeftThrust(true);
        vehicleInputData.setDigitalRightThrust(true);
    }
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        if(!bStartedLeftTurn || !bStartedRightTurn)
        {
            vehicleInputData.setDigitalAccel(false);

            if(!bStartedLeftTurn)
                vehicleInputData.setDigitalLeftThrust(false);

            if(!bStartedRightTurn)
                vehicleInputData.setDigitalRightThrust(false);
        }
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        startBrakeMode();
    }
    else if(key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        if(bBreakedLeft)
        {
            vehicleInputData.setDigitalLeftBrake(false);
            bBreakedLeft = false;
        }

        if(bBreakedRight)
        {
            vehicleInputData.setDigitalRightBrake(false);
            bBreakedRight = false;
        }
    }
}

void engine::physics::components::CVehicleTank::processInput(const GLFWgamepadstate& controllerState)
{
    bIsControlledByController = true;
    float acceleration = controllerState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
    float brake = controllerState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
    float steering = controllerState.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    vehicleInputData.setAnalogAccel(mapInputToRange(acceleration, INPUT_RANGE, OUTPUT_RANGE));
    vehicleInputData.setAnalogLeftThrust(mapInputToRange(acceleration, INPUT_RANGE, OUTPUT_RANGE));
    vehicleInputData.setAnalogRightThrust(mapInputToRange(acceleration, INPUT_RANGE, OUTPUT_RANGE));
    vehicleInputData.setAnalogLeftBrake(mapInputToRange(brake, INPUT_RANGE, OUTPUT_RANGE));

    if(steering < -0.1f)
    {
        vehicleInputData.setAnalogLeftBrake(abs(steering));
    }
    else if(steering > 0.1f)
    {
        vehicleInputData.setAnalogRightBrake(steering);
    }
    else
    {
        vehicleInputData.setAnalogLeftBrake(0.f);
        vehicleInputData.setAnalogRightBrake(0.f);
    }
}

void engine::physics::components::CVehicleTank::processAutoReverse(bool& bToggleAutoReverse, bool& bNewIsMovingForwardSlowly)
{
    physx::PxVehicleDriveDynData* dynData = &vehicleTank->mDriveDynData;
    const physx::PxF32 forwardSpeed = vehicleTank->computeForwardSpeed();
    const physx::PxF32 forwardSpeedAbs = physx::PxAbs(forwardSpeed);
    const physx::PxF32 sidewaysSpeed = vehicleTank->computeSidewaysSpeed();
    const physx::PxF32 sidewaysSpeedAbs = physx::PxAbs(sidewaysSpeed);
    const physx::PxU32 currentGear = dynData->getCurrentGear();
    const physx::PxU32 targetGear =  dynData->getTargetGear();
    const bool prevIsMovingForwardSlowly = bIsMovingForwardSlowly;
    bool bIsMovingForwardSlowlyLocal = false;
    bool bIsMovingBackwards = false;

    bool brake = vehicleInputData.getDigitalLeftBrake() && vehicleInputData.getDigitalRightBrake();
    bool accel = vehicleInputData.getDigitalAccel();

    //Check if the car is rolling against the gear (backwards in forward gear or forwards in reverse gear).
    if(physx::PxVehicleGearsData::eFIRST == currentGear && forwardSpeed < -THRESHOLD_ROLLING_BACKWARDS_SPEED)
    {
        bIsMovingBackwards = true;
    }
    else if(physx::PxVehicleGearsData::eREVERSE == currentGear && forwardSpeed > THRESHOLD_ROLLING_BACKWARDS_SPEED)
    {
        bIsMovingBackwards = true;
    }

    //Check if the car is moving slowly.
    if(forwardSpeedAbs < THRESHOLD_FORWARD_SPEED && sidewaysSpeedAbs < THRESHOLD_SIDEWAYS_SPEED)
    {
        bIsMovingForwardSlowlyLocal = true;
    }

    //Now work if we need to toggle from forwards gear to reverse gear or vice versa.
    if(bIsMovingBackwards)
    {
        if(!accel && !brake && (currentGear == targetGear))
        {
            //The car is rolling against the gear and the player is doing nothing to stop this.
            bToggleAutoReverse = true;
        }
    }
    else if(prevIsMovingForwardSlowly && bIsMovingForwardSlowlyLocal)
    {
        if((currentGear > physx::PxVehicleGearsData::eNEUTRAL) && brake && !accel && (currentGear == targetGear))
        {
            //The car was moving slowly in forward gear without player input and is now moving slowly with player input that indicates the
            //player wants to switch to reverse gear.
            bToggleAutoReverse = true;
        }
        else if(currentGear == physx::PxVehicleGearsData::eREVERSE && accel && !brake && (currentGear == targetGear))
        {
            //The car was moving slowly in reverse gear without player input and is now moving slowly with player input that indicates the
            //player wants to switch to forward gear.
            bToggleAutoReverse = true;
        }
    }

    //If the car was brought to rest through braking then the player needs to release the brake then reapply
    //to indicate that the gears should toggle between reverse and forward.
    if(bIsMovingForwardSlowlyLocal && !brake && !accel)
    {
        bNewIsMovingForwardSlowly = true;
    }
}

void
engine::physics::components::CVehicleTank::updateDebugInfo(engine::physics::vehicle::DebugVehicleTank& dbgInfo)
{
    const physx::PxVehicleDriveDynData* dynData = &vehicle->mDriveDynData;
    const physx::PxVehicleWheelsSimData* wheelsSimData = &vehicle->mWheelsSimData;
    const physx::PxVehicleSuspensionData suspensionData = wheelsSimData->getSuspensionData(0);

    /* Vehicle Description */
    dbgInfo.chassisMass = vehicle->getRigidDynamicActor()->getMass();
    dbgInfo.chassisDims[0] = vehicleDesc.chassisDims[0];
    dbgInfo.chassisDims[1] = vehicleDesc.chassisDims[1];
    dbgInfo.chassisDims[2] = vehicleDesc.chassisDims[2];
    dbgInfo.chassisMOI[0] = vehicleDesc.chassisMOI[0];
    dbgInfo.chassisMOI[1] = vehicleDesc.chassisMOI[1];
    dbgInfo.chassisMOI[2] = vehicleDesc.chassisMOI[2];
    dbgInfo.chassisCMOffset[0] = vehicleDesc.chassisCMOffset[0];
    dbgInfo.chassisCMOffset[1] = vehicleDesc.chassisCMOffset[1];
    dbgInfo.chassisCMOffset[2] = vehicleDesc.chassisCMOffset[2];
    dbgInfo.wheelMass = vehicleDesc.wheelMass;
    dbgInfo.wheelRadius = vehicleDesc.wheelRadius;
    dbgInfo.wheelWidth = vehicleDesc.wheelWidth;
    dbgInfo.wheelMOI = vehicleDesc.wheelMOI;

    /* Suspension */
    dbgInfo.maxCompression = suspensionData.mMaxCompression;
    dbgInfo.maxDroop = suspensionData.mMaxDroop;
    dbgInfo.springStrength = suspensionData.mSpringStrength;
    dbgInfo.springDamperRate = suspensionData.mSpringDamperRate;
    dbgInfo.sprungMass = suspensionData.mSprungMass;

    /* Drive Info */
    dbgInfo.forwardSpeed = vehicle->computeForwardSpeed();
    dbgInfo.sidewaysSpeed = vehicle->computeSidewaysSpeed();
    dbgInfo.forwardSpeedAbs = physx::PxAbs(dbgInfo.forwardSpeed);
    dbgInfo.sidewaysSpeedAbs = physx::PxAbs(dbgInfo.sidewaysSpeed);
    dbgInfo.currentGear = dynData->mCurrentGear;
    dbgInfo.targetGear = dynData->mTargetGear;
    dbgInfo.bIsInReverse = bIsInReverse;

    /* Keyboard Input */
    dbgInfo.bIsAccelerating = vehicleInputData.getDigitalAccel();
    dbgInfo.bThrustLeft = vehicleInputData.getDigitalLeftThrust();
    dbgInfo.bThrustRight = vehicleInputData.getDigitalRightThrust();
    dbgInfo.bBreakLeft = vehicleInputData.getDigitalLeftBrake();
    dbgInfo.bBreakRight = vehicleInputData.getDigitalRightBrake();
    dbgInfo.bIsBraking = vehicleInputData.getDigitalLeftBrake() && vehicleInputData.getDigitalRightBrake();
}

void
engine::physics::components::CVehicleTank::debugVehicle(const engine::physics::vehicle::DebugVehicleTank& debugInfo)
{
    pxScene->removeActor(*vehicleTank->getRigidDynamicActor());
    vehicleTank->release();

    vehicleTank = createVehicle();
    vehicle = vehicleTank;

    //Configure the userdata
    configureUserData();

    transform = glm::translate(transform,
                               glm::vec3(0, vehicleDesc.chassisDims.y * 0.25f + vehicleDesc.wheelRadius + 1.0f, 0.f));

    vehicleTank->getRigidDynamicActor()->setGlobalPose(glmToPhysx(transform));
    pxScene->addActor(*vehicleTank->getRigidDynamicActor());

    //Set the vehicle to rest in first gear.
    //Set the vehicle to use auto-gears.
    vehicleTank->setToRestState();
    vehicleTank->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
    vehicleTank->mDriveDynData.setUseAutoGears(true);
    vehicleTank->setDriveModel(physx::PxVehicleDriveTankControlModel::eSTANDARD);
}

engine::physics::vehicle::DebugVehicleTank engine::physics::components::CVehicleTank::getDebugInfo()
{
    engine::physics::vehicle::DebugVehicleTank dbgInfo;

    physx::PxVehicleSuspensionData suspensionData = vehicleTank->mWheelsSimData.getSuspensionData(0);
    physx::PxVehicleDriveSimData driveSimData = vehicleTank->mDriveSimData;
    physx::PxVehicleEngineData engineData = driveSimData.getEngineData();
    physx::PxVehicleGearsData gearData = driveSimData.getGearsData();
    physx::PxVehicleClutchData clutchData = driveSimData.getClutchData();
    physx::PxVehicleDriveDynData* dynData = &vehicleTank->mDriveDynData;

    /* Vehicle Description */
    dbgInfo.chassisMass = vehicle->getRigidDynamicActor()->getMass();
    dbgInfo.chassisDims[0] = vehicleDesc.chassisDims[0];
    dbgInfo.chassisDims[1] = vehicleDesc.chassisDims[1];
    dbgInfo.chassisDims[2] = vehicleDesc.chassisDims[2];
    dbgInfo.chassisMOI[0] = vehicleDesc.chassisMOI[0];
    dbgInfo.chassisMOI[1] = vehicleDesc.chassisMOI[1];
    dbgInfo.chassisMOI[2] = vehicleDesc.chassisMOI[2];
    dbgInfo.chassisCMOffset[0] = vehicleDesc.chassisCMOffset[0];
    dbgInfo.chassisCMOffset[1] = vehicleDesc.chassisCMOffset[1];
    dbgInfo.chassisCMOffset[2] = vehicleDesc.chassisCMOffset[2];
    dbgInfo.wheelMass = vehicleDesc.wheelMass;
    dbgInfo.wheelRadius = vehicleDesc.wheelRadius;
    dbgInfo.wheelWidth = vehicleDesc.wheelWidth;
    dbgInfo.wheelMOI = vehicleDesc.wheelMOI;

    /* Suspension */
    dbgInfo.maxCompression = suspensionData.mMaxCompression;
    dbgInfo.maxDroop = suspensionData.mMaxDroop;
    dbgInfo.springStrength = suspensionData.mSpringStrength;
    dbgInfo.springDamperRate = suspensionData.mSpringDamperRate;
    dbgInfo.sprungMass = suspensionData.mSprungMass;

    /* Drive Sim Data */
    dbgInfo.enginPeakTorq = engineData.mPeakTorque;
    dbgInfo.maxOmega = engineData.mMaxOmega;
    dbgInfo.gearSwitchTime = gearData.mSwitchTime;
    dbgInfo.clutchStrength = clutchData.mStrength;

    /* Physics Materials */
    dbgInfo.drivableMat[0] = chassisMaterialDrivable->getStaticFriction();
    dbgInfo.drivableMat[1] = chassisMaterialDrivable->getDynamicFriction();
    dbgInfo.drivableMat[2] = chassisMaterialDrivable->getRestitution();
    dbgInfo.nonDrivableMat[0] = chassisMaterialNonDrivable->getStaticFriction();
    dbgInfo.nonDrivableMat[1] = chassisMaterialNonDrivable->getDynamicFriction();
    dbgInfo.nonDrivableMat[2] = chassisMaterialNonDrivable->getRestitution();

    /* Drive Info */
    dbgInfo.forwardSpeed = vehicle->computeForwardSpeed();
    dbgInfo.sidewaysSpeed = vehicle->computeSidewaysSpeed();
    dbgInfo.forwardSpeedAbs = physx::PxAbs(dbgInfo.forwardSpeed);
    dbgInfo.sidewaysSpeedAbs = physx::PxAbs(dbgInfo.sidewaysSpeed);
    dbgInfo.currentGear = dynData->mCurrentGear;
    dbgInfo.targetGear = dynData->mTargetGear;
    dbgInfo.bIsInReverse = bIsInReverse;

    /* Keyboard Input */
    dbgInfo.bIsAccelerating = vehicleInputData.getDigitalAccel();
    dbgInfo.bThrustLeft = vehicleInputData.getDigitalLeftThrust();
    dbgInfo.bThrustRight = vehicleInputData.getDigitalRightThrust();
    dbgInfo.bBreakLeft = vehicleInputData.getDigitalLeftBrake();
    dbgInfo.bBreakRight = vehicleInputData.getDigitalRightBrake();
    dbgInfo.bIsBraking = vehicleInputData.getDigitalLeftBrake() && vehicleInputData.getDigitalRightBrake();

    return dbgInfo;
}

