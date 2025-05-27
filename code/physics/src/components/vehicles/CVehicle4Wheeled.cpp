//
// Created by CemOe on 19.09.2021.
//

#include "engine/physics/components/vehicles/CVehicle4Wheeled.h"
#include "engine/scene/components/CTransform.h"
#include "engine/scene/TreeTraverser.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "engine/physics/Physics.h"

#define THRESHOLD_FORWARD_SPEED (0.1f)
#define THRESHOLD_SIDEWAYS_SPEED (0.2f)
#define THRESHOLD_ROLLING_BACKWARDS_SPEED (0.1f)
#define NUMBER_OF_WHEELS (4)

namespace engine::physics::vehicle
{
    PxVehicleKeySmoothingData keySmoothingData =
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

    PxVehiclePadSmoothingData padSmoothingData =
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

    PxF32 steerVsForwardSpeedData[2 * 8] =
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
    PxFixedSizeLookupTable<8> steerVsForwardSpeedTable(steerVsForwardSpeedData, 4);
}

engine::physics::components::CVehicle4Wheeled::CVehicle4Wheeled(
        engine::components::id_t id, engine::entities::id_t owner) : CVehicle(id, owner)
{

}

void engine::physics::components::CVehicle4Wheeled::createVehiclePhysics()
{
    if (pxScene && !bIsActive)
    {
        bIsActive = true;

        vehicle4W = createVehicle();
        vehicle = vehicle4W;

        configureUserData();

        transform = glm::translate(transform,
                                   glm::vec3(0, vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f, 0.f));
        vehicle4W->getRigidDynamicActor()->setGlobalPose(glmToPhysx(transform));
        pxScene->addActor(*vehicle4W->getRigidDynamicActor());

        //Set the vehicle to rest in first gear.
        //Set the vehicle to use auto-gears.
        vehicle4W->setToRestState();
        vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eNEUTRAL);
        vehicle4W->mDriveDynData.setUseAutoGears(true);

        fireEvent(event_physicsInitialized, &vehicle);
    }
}

physx::PxVehicleDrive4W* engine::physics::components::CVehicle4Wheeled::createVehicle()
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    glm::vec3 euler = glm::eulerAngles(rotation);

    vehicleDesc.chassisDims = physx::PxVec3(vehicleDesc.chassisDims.x * scale.x,
                                     vehicleDesc.chassisDims.y * scale.y,
                                     vehicleDesc.chassisDims.z * scale.z);

    vehicleDesc.chassisMOI = physx::PxVec3(vehicleDesc.chassisMOI.x * scale.x,
                                    vehicleDesc.chassisMOI.y * scale.y,
                                    vehicleDesc.chassisMOI.z * scale.z);

    vehicleDesc.chassisCMOffset = physx::PxVec3(0.0f,
                                                (-vehicleDesc.chassisDims.y * 0.5f + 0.65f) * scale.y,
                                                0.25f * scale.z);

    vehicleDesc.wheelRadius *= scale.y;
    vehicleDesc.wheelWidth *=  scale.x;
    vehicleDesc.wheelMOI = 0.5f * vehicleDesc.wheelMass * vehicleDesc.wheelRadius * vehicleDesc.wheelRadius;

    const physx::PxVec3 chassisDims = vehicleDesc.chassisDims;
    const physx::PxF32 wheelWidth = vehicleDesc.wheelWidth;
    const physx::PxF32 wheelRadius = vehicleDesc.wheelRadius;
    const physx::PxU32 numWheels = vehicleDesc.numWheels;

    const physx::PxFilterData& chassisSimFilterData = vehicleDesc.chassisSimFilterData;
    const physx::PxFilterData& wheelSimFilterData = vehicleDesc.wheelSimFilterData;

    //Construct a physx actor with shapes for the chassis and wheels.
    //Set the rigid body mass, moment of inertia, and center of mass offset.
    physx::PxRigidDynamic* veh4WActor = nullptr;
    {
        //Construct a convex mesh for a cylindrical wheel.
        physx::PxConvexMesh* wheelMesh = createWheelMesh(wheelWidth, wheelRadius);

        //Assume all wheels are identical for simplicity.
        physx::PxConvexMesh* wheelConvexMeshes[NUMBER_OF_WHEELS];
        physx::PxMaterial* wheelMaterials[NUMBER_OF_WHEELS];

        chassisMaterialDrivable = physicsSystem->getChassisMaterialDrivable();
        chassisMaterialNonDrivable = physicsSystem->getChassisMaterialNonDrivable();

        //Set the meshes and materials for the driven wheels.
        for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
        {
            wheelConvexMeshes[i] = wheelMesh;
            wheelMaterials[i] = chassisMaterialDrivable;
        }


        //Set the meshes and materials for the non-driven wheels
        for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
        {
            wheelConvexMeshes[i] = wheelMesh;
            wheelMaterials[i] = chassisMaterialNonDrivable;
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

        veh4WActor = createVehicleActor(rigidBodyData, wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData,
                                        chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData);
    }

    //Set up the sim data for the wheels.
    physx::PxVehicleWheelsSimData* wheelsSimData = physx::PxVehicleWheelsSimData::allocate(numWheels);
    {
        //Compute the wheel center offsets from the origin.
        physx::PxVec3 wheelCenterActorOffsets[NUMBER_OF_WHEELS];
        const physx::PxF32 frontZ = chassisDims.z * 0.3f;
        const physx::PxF32 rearZ = -chassisDims.z * 0.3f;
        computeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels,
                                         wheelCenterActorOffsets);

        //Set up the simulation data for all wheels.
        setupWheels
                (vehicleDesc.wheelMass, vehicleDesc.wheelMOI, wheelRadius, wheelWidth,
                 numWheels, wheelCenterActorOffsets,
                 vehicleDesc.chassisCMOffset, vehicleDesc.chassisMass,
                 wheelsSimData);
    }

    //Set up the sim data for the vehicle drive model.
    physx::PxVehicleDriveSimData4W driveSimData;
    {
        //Diff
        physx::PxVehicleDifferential4WData diff;
        diff.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
        driveSimData.setDiffData(diff);

        //Engine
        physx::PxVehicleEngineData engine;
        engine.mPeakTorque = 500.0f;
        //approx 6000 rpm
        engine.mMaxOmega = 600.0f;
        driveSimData.setEngineData(engine);

        //Gears
        physx::PxVehicleGearsData gears;
        gears.mSwitchTime = 0.5f;
        driveSimData.setGearsData(gears);

        //Clutch
        physx::PxVehicleClutchData clutch;
        clutch.mStrength = 10.0f;
        driveSimData.setClutchData(clutch);

        //Ackermann steer accuracy
        physx::PxVehicleAckermannGeometryData ackermann;
        ackermann.mAccuracy = 1.0f;
        ackermann.mAxleSeparation =
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
        ackermann.mFrontWidth =
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
        ackermann.mRearWidth =
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
        driveSimData.setAckermannGeometryData(ackermann);
    }

    //Create a vehicle from the wheels and drive sim data.
    physx::PxVehicleDrive4W* vehDrive4W = physx::PxVehicleDrive4W::allocate(numWheels);
    vehDrive4W->setup(pxPhysics, veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);

    //Free the sim data because we don't need that any more.
    wheelsSimData->free();

    return vehDrive4W;
}

void engine::physics::components::CVehicle4Wheeled::computeWheelCenterActorOffsets4W(const float wheelFrontZ,
                                                                                     const float wheelRearZ,
                                                                                     const physx::PxVec3 chassisDims,
                                                                                     const float wheelWidth,
                                                                                     const float wheelRadius,
                                                                                     const unsigned int numWheels,
                                                                                     physx::PxVec3* wheelCentreOffsets)
{
    //chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
    //The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
    //Compute a position for the front wheel and the rear wheel along the z-axis.
    //Compute the separation between each wheel along the z-axis.
    const physx::PxF32 numLeftWheels = numWheels / 2.0f;
    const physx::PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f);
    //Set the outside of the left and right wheels to be flush with the chassis.
    //Set the top of the wheel to be just touching the underside of the chassis.
    //Begin by setting the rear-left/rear-right/front-left,front-right wheels.
    wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] = physx::PxVec3(((-chassisDims.x + wheelWidth) * 0.5f),
                                                                        -(chassisDims.y / 2 + wheelRadius),
                                                                        wheelRearZ + 0 * deltaZ * 0.5f);
    wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth) * 0.5f,
                                                                         -(chassisDims.y / 2 + wheelRadius),
                                                                         wheelRearZ + 0 * deltaZ * 0.5f);
    wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = physx::PxVec3((-chassisDims.x + wheelWidth) * 0.5f,
                                                                         -(chassisDims.y / 2 + wheelRadius),
                                                                         wheelRearZ + (numLeftWheels - 1) * deltaZ);
    wheelCentreOffsets[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = physx::PxVec3((+chassisDims.x - wheelWidth) * 0.5f,
                                                                          -(chassisDims.y / 2 + wheelRadius),
                                                                          wheelRearZ + (numLeftWheels - 1) * deltaZ);
    //Set the remaining wheels.
    for (physx::PxU32 i = 2, wheelCount = 4; i < numWheels - 2; i += 2, wheelCount += 2)
    {
        wheelCentreOffsets[wheelCount + 0] = physx::PxVec3((-chassisDims.x + wheelWidth) * 0.5f,
                                                    -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
        wheelCentreOffsets[wheelCount + 1] = physx::PxVec3((+chassisDims.x - wheelWidth) * 0.5f,
                                                    -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f);
    }
}

void engine::physics::components::CVehicle4Wheeled::setupWheels(float wheelMass, float wheelMOI,
                                                                const float wheelRadius,
                                                                const float wheelWidth,
                                                                const unsigned int numWheels,
                                                                physx::PxVec3* wheelCenterActorOffsets,
                                                                physx::PxVec3 chassisCMOffset,
                                                                float chassisMass,
                                                                physx::PxVehicleWheelsSimData* wheelsSimData)
{
    //Set up the wheels.
    physx::PxVehicleWheelData wheels[NUMBER_OF_WHEELS];
    {
        //Set up the wheel data structures with mass, moi, radius, width.
        for (physx::PxU32 i = 0; i < numWheels; i++)
        {
            wheels[i].mMass = wheelMass;
            wheels[i].mMOI = wheelMOI;
            wheels[i].mRadius = wheelRadius;
            wheels[i].mWidth = wheelWidth;
            wheels[i].mDampingRate = 2.f;
        }

        //Enable the handbrake for the rear wheels only.
        wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
        wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
        //Enable steering for the front wheels only.
        wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = physx::PxPi * 0.3333f;
        wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = physx::PxPi * 0.3333f;
    }

    //Set up the tires.
    physx::PxVehicleTireData tires[NUMBER_OF_WHEELS];
    {
        //Set up the tires.
        for (physx::PxU32 i = 0; i < numWheels; i++)
            tires[i].mType = physics::vehicle::TireType::TIRE_TYPE_NORMAL;
    }

    //Set up the suspensions
    physx::PxVehicleSuspensionData suspensions[NUMBER_OF_WHEELS];
    {
        //Compute the mass supported by each suspension spring.
        physx::PxF32 suspSprungMasses[NUMBER_OF_WHEELS];
        PxVehicleComputeSprungMasses(numWheels, wheelCenterActorOffsets,
                                     chassisCMOffset, chassisMass, 1, suspSprungMasses);

        //Set the suspension data.
        for (physx::PxU32 i = 0; i < numWheels; i++)
        {
            suspensions[i].mMaxCompression = 0.3f;
            suspensions[i].mMaxDroop = 0.1f;
            suspensions[i].mSpringStrength = 35000.0f;
            suspensions[i].mSpringDamperRate = 4500.0f;
            suspensions[i].mSprungMass = suspSprungMasses[i];
        }

        //Set the camber angles.
        const physx::PxF32 camberAngleAtRest = 0.0;
        const physx::PxF32 camberAngleAtMaxDroop = 0.001f;
        const physx::PxF32 camberAngleAtMaxCompression = -0.001f;
        for (physx::PxU32 i = 0; i < numWheels; i += 2)
        {
            suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
            suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
            suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
            suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
            suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
            suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
        }
    }

    //Set up the wheel geometry.
    physx::PxVec3 suspTravelDirections[NUMBER_OF_WHEELS];
    physx::PxVec3 wheelCentreCMOffsets[NUMBER_OF_WHEELS];
    physx::PxVec3 suspForceAppCMOffsets[NUMBER_OF_WHEELS];
    physx::PxVec3 tireForceAppCMOffsets[NUMBER_OF_WHEELS];
    {
        //Set the geometry data.
        for (physx::PxU32 i = 0; i < numWheels; i++)
        {
            //Vertical suspension travel.
            suspTravelDirections[i] = physx::PxVec3(0, -1, 0);

            //Wheel center offset is offset from rigid body center of mass.
            wheelCentreCMOffsets[i] =
                    wheelCenterActorOffsets[i] - chassisCMOffset;

            //Suspension force application point 0.3 metres below
            //rigid body center of mass.
            suspForceAppCMOffsets[i] =
                    physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

            //Tire force application point 0.3 metres below
            //rigid body center of mass.
            tireForceAppCMOffsets[i] =
                    physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
        }
    }

    //Set up the filter data of the raycast that will be issued by each suspension.
    physx::PxFilterData qryFilterData;
    qryFilterData.word3 = physics::vehicle::DRIVABLE_SURFACE;

    //Set the wheel, tire and suspension data.
    //Set the geometry data.
    //Set the query filter data
    for (physx::PxU32 i = 0; i < numWheels; i++)
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

    //Add a front and rear anti-roll bar
    physx::PxVehicleAntiRollBarData barFront;
    barFront.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
    barFront.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
    barFront.mStiffness = 10000.0f;
    wheelsSimData->addAntiRollBarData(barFront);
    physx::PxVehicleAntiRollBarData barRear;
    barRear.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT;
    barRear.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
    barRear.mStiffness = 10000.0f;
    wheelsSimData->addAntiRollBarData(barRear);
}

void engine::physics::components::CVehicle4Wheeled::tick(float deltaTime)
{
    engine::physics::components::CVehicle::tick(deltaTime);

    physx::PxVehicleDriveDynData* driveDynData = &vehicle4W->mDriveDynData;
    physx::PxVehicleDrive4WRawInputData newInputData = physx::PxVehicleDrive4WRawInputData();

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
        const bool brake = newInputData.getDigitalBrake();
        newInputData.setDigitalAccel(brake);
        newInputData.setDigitalBrake(accel);
    }

    if(!bIsControlledByController)
    {
        physx::PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(engine::physics::vehicle::keySmoothingData,
                                                                        engine::physics::vehicle::steerVsForwardSpeedTable,
                                                                        newInputData, deltaTime,
                                                                        bIsVehicleInAir, *vehicle4W);
    }
    else
    {
        physx::PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(engine::physics::vehicle::padSmoothingData,
                                                                       engine::physics::vehicle::steerVsForwardSpeedTable,
                                                                       newInputData,deltaTime,
                                                                       bIsVehicleInAir, *vehicle4W);
    }

    updateTransform();
}

void
engine::physics::components::CVehicle4Wheeled::processInput(const GLFWgamepadstate& controllerState)
{
    bIsControlledByController = true;
    float acceleration = controllerState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
    float brake = controllerState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
    float steering = controllerState.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    vehicleInputData.setAnalogAccel(mapInputToRange(acceleration, INPUT_RANGE, OUTPUT_RANGE));
    vehicleInputData.setAnalogBrake(mapInputToRange(brake, INPUT_RANGE, OUTPUT_RANGE));
    vehicleInputData.setAnalogSteer(steering);
}

void engine::physics::components::CVehicle4Wheeled::processInput(int key, int action)
{
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        vehicleInputData.setDigitalSteerRight(true);
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        vehicleInputData.setDigitalSteerRight(false);

    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        vehicleInputData.setDigitalSteerLeft(true);
    else if(key == GLFW_KEY_D && action == GLFW_RELEASE)
        vehicleInputData.setDigitalSteerLeft(false);

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        vehicleInputData.setDigitalAccel(true);
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        vehicleInputData.setDigitalAccel(false);

    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        vehicleInputData.setDigitalBrake(true);
    else if(key == GLFW_KEY_S && action == GLFW_RELEASE)
        vehicleInputData.setDigitalBrake(false);
}

void engine::physics::components::CVehicle4Wheeled::processAutoReverse(bool& bToggleAutoReverse, bool& bNewIsMovingForwardSlowly)
{
    physx::PxVehicleDriveDynData* dynData = &vehicle4W->mDriveDynData;
    const physx::PxF32 forwardSpeed = vehicle4W->computeForwardSpeed();
    const physx::PxF32 forwardSpeedAbs = physx::PxAbs(forwardSpeed);
    const physx::PxF32 sidewaysSpeed = vehicle4W->computeSidewaysSpeed();
    const physx::PxF32 sidewaysSpeedAbs = physx::PxAbs(sidewaysSpeed);
    const physx::PxU32 currentGear = dynData->getCurrentGear();
    const physx::PxU32 targetGear =  dynData->getTargetGear();
    const bool prevIsMovingForwardSlowly = bIsMovingForwardSlowly;
    bool bIsMovingForwardSlowlyLocal = false;
    bool bIsMovingBackwards = false;

    if(bIsVehicleInAir)
        return;

    bool brake = vehicleInputData.getDigitalBrake();
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

engine::physics::vehicle::DebugRaycastVehicle4W engine::physics::components::CVehicle4Wheeled::getDebugInfo()
{
    engine::physics::vehicle::DebugRaycastVehicle4W dbgInfo {};
    physx::PxVehicleSuspensionData suspensionData = vehicle4W->mWheelsSimData.getSuspensionData(0);
    physx::PxVehicleDriveSimData4W driveSimData = vehicle4W->mDriveSimData;
    physx::PxVehicleEngineData engineData = driveSimData.getEngineData();
    physx::PxVehicleGearsData gearData = driveSimData.getGearsData();
    physx::PxVehicleClutchData clutchData = driveSimData.getClutchData();
    physx::PxVehicleAckermannGeometryData ackermannData = driveSimData.getAckermannGeometryData();
    physx::PxVehicleDriveDynData* dynData = &vehicle4W->mDriveDynData;

    /* Vehicle Description */
    dbgInfo.chassisMass = vehicleDesc.chassisMass;
    dbgInfo.chassisDims[0] = vehicleDesc.chassisDims.x;
    dbgInfo.chassisDims[1] = vehicleDesc.chassisDims.y;
    dbgInfo.chassisDims[2] = vehicleDesc.chassisDims.z;
    dbgInfo.chassisMOI[0] = vehicleDesc.chassisMOI.x;
    dbgInfo.chassisMOI[1] = vehicleDesc.chassisMOI.y;
    dbgInfo.chassisMOI[2] = vehicleDesc.chassisMOI.z;
    dbgInfo.chassisCMOffset[0] = vehicleDesc.chassisCMOffset.x;
    dbgInfo.chassisCMOffset[1] = vehicleDesc.chassisCMOffset.y;
    dbgInfo.chassisCMOffset[2] = vehicleDesc.chassisCMOffset.z;
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
    dbgInfo.camberAtRest = suspensionData.mCamberAtRest;
    dbgInfo.camberAtMaxDroop = suspensionData.mCamberAtMaxDroop;
    dbgInfo.camberAtMaxCompression = suspensionData.mCamberAtMaxCompression;

    /* Drive Sim Data */
    dbgInfo.enginPeakTorq = engineData.mPeakTorque;
    dbgInfo.maxOmega = engineData.mMaxOmega;
    dbgInfo.gearSwitchTime = gearData.mSwitchTime;
    dbgInfo.clutchStrength = clutchData.mStrength;
    dbgInfo.ackermannAcuracy = ackermannData.mAccuracy;
    dbgInfo.ackermannFrontWidth = ackermannData.mFrontWidth;
    dbgInfo.ackermannRearWidth = ackermannData.mRearWidth;
    dbgInfo.ackermannAxleSeparation = ackermannData.mAxleSeparation;

    /* Physics Materials */
    dbgInfo.drivableMat[0] = chassisMaterialDrivable->getStaticFriction();
    dbgInfo.drivableMat[1] = chassisMaterialDrivable->getDynamicFriction();
    dbgInfo.drivableMat[2] = chassisMaterialDrivable->getRestitution();
    dbgInfo.nonDrivableMat[0] = chassisMaterialNonDrivable->getStaticFriction();
    dbgInfo.nonDrivableMat[1] = chassisMaterialNonDrivable->getDynamicFriction();
    dbgInfo.nonDrivableMat[2] = chassisMaterialNonDrivable->getRestitution();

    /* Drive Info */
    dbgInfo.forwardSpeed = vehicle4W->computeForwardSpeed();
    dbgInfo.sidewaysSpeed = vehicle4W->computeSidewaysSpeed();
    dbgInfo.forwardSpeedAbs = physx::PxAbs(dbgInfo.forwardSpeed);
    dbgInfo.sidewaysSpeedAbs = physx::PxAbs(dbgInfo.sidewaysSpeed);
    dbgInfo.currentGear = dynData->mCurrentGear;
    dbgInfo.targetGear = dynData->mTargetGear;
    dbgInfo.bIsInReverse = bIsInReverse;

    /* Keyboard Input */
    dbgInfo.bIsAccelerating = vehicleInputData.getDigitalAccel();
    dbgInfo.bIsBraking = vehicleInputData.getDigitalBrake();

    return dbgInfo;
}

void
engine::physics::components::CVehicle4Wheeled::debugVehicle(engine::physics::vehicle::DebugRaycastVehicle4W& dbgInfo)
{
    physx::PxTransform globalTransform = vehicle4W->getRigidDynamicActor()->getGlobalPose();
    pxScene->removeActor(*vehicle4W->getRigidDynamicActor());
    vehicle->release();

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    glm::vec3 euler = glm::eulerAngles(rotation);

    vehicleDesc.chassisDims = physx::PxVec3(dbgInfo.chassisDims[0] * scale.x,
                                     dbgInfo.chassisDims[1] * scale.y,
                                     dbgInfo.chassisDims[2] * scale.z);

    vehicleDesc.chassisMOI = physx::PxVec3(dbgInfo.chassisMOI[0] * scale.x,
                                    dbgInfo.chassisMOI[1] * scale.y,
                                    dbgInfo.chassisMOI[2] * scale.z);

    vehicleDesc.chassisCMOffset = physx::PxVec3(0.0f,
                                                (-dbgInfo.chassisDims[1] * 0.5f + 0.65f) * scale.y,
                                                0.25f * scale.z);

    vehicleDesc.wheelRadius = dbgInfo.wheelRadius;
    vehicleDesc.wheelWidth = dbgInfo.wheelWidth;
    vehicleDesc.wheelRadius *= scale.y;
    vehicleDesc.wheelWidth *= scale.x;
    vehicleDesc.wheelMOI = 0.5f * dbgInfo.wheelMass * dbgInfo.wheelRadius * dbgInfo.wheelRadius;

    const physx::PxVec3 chassisDims = physx::PxVec3(dbgInfo.chassisDims[0], dbgInfo.chassisDims[1], dbgInfo.chassisDims[2]);
    const physx::PxF32 wheelWidth = vehicleDesc.wheelWidth;
    const physx::PxF32 wheelRadius = vehicleDesc.wheelRadius;
    const physx::PxU32 numWheels = vehicleDesc.numWheels;

    const physx::PxFilterData& chassisSimFilterData = vehicleDesc.chassisSimFilterData;
    const physx::PxFilterData& wheelSimFilterData = vehicleDesc.wheelSimFilterData;

    //Construct a physx actor with shapes for the chassis and wheels.
    //Set the rigid body mass, moment of inertia, and center of mass offset.
    physx::PxRigidDynamic* veh4WActor = nullptr;
    {
        //Construct a convex mesh for a cylindrical wheel.
        physx::PxConvexMesh* wheelMesh = createWheelMesh(wheelWidth, wheelRadius);

        //Assume all wheels are identical for simplicity.
        physx::PxConvexMesh* wheelConvexMeshes[NUMBER_OF_WHEELS];
        physx::PxMaterial* wheelMaterials[NUMBER_OF_WHEELS];

        chassisMaterialDrivable->setStaticFriction(dbgInfo.drivableMat[0]);
        chassisMaterialDrivable->setDynamicFriction(dbgInfo.drivableMat[1]);
        chassisMaterialDrivable->setRestitution(dbgInfo.drivableMat[2]);
        chassisMaterialNonDrivable->setStaticFriction(dbgInfo.nonDrivableMat[0]);
        chassisMaterialNonDrivable->setDynamicFriction(dbgInfo.nonDrivableMat[1]);
        chassisMaterialNonDrivable->setRestitution(dbgInfo.nonDrivableMat[2]);

        //Set the meshes and materials for the driven wheels.
        for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
        {
            wheelConvexMeshes[i] = wheelMesh;
            wheelMaterials[i] = chassisMaterialDrivable;
        }

        //Set the meshes and materials for the non-driven wheels
        for (physx::PxU32 i = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
        {
            wheelConvexMeshes[i] = wheelMesh;
            wheelMaterials[i] = chassisMaterialNonDrivable;
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

        veh4WActor = createVehicleActor(rigidBodyData, wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData,
                                        chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData);
    }

    //Set up the sim data for the wheels.
    physx::PxVehicleWheelsSimData* wheelsSimData = physx::PxVehicleWheelsSimData::allocate(numWheels);
    {
        //Compute the wheel center offsets from the origin.
        physx::PxVec3 wheelCenterActorOffsets[NUMBER_OF_WHEELS];
        const physx::PxF32 frontZ = chassisDims.z * 0.3f;
        const physx::PxF32 rearZ = -chassisDims.z * 0.3f;
        computeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels,
                                         wheelCenterActorOffsets);

        //Set up the simulation data for all wheels.
        //Set up the wheels.
        physx::PxVehicleWheelData wheels[NUMBER_OF_WHEELS];
        {
            //Set up the wheel data structures with mass, moi, radius, width.
            for (physx::PxU32 i = 0; i < numWheels; i++)
            {
                wheels[i].mMass = vehicleDesc.wheelMass;
                wheels[i].mMOI = vehicleDesc.wheelMOI;
                wheels[i].mRadius = wheelRadius;
                wheels[i].mWidth = wheelWidth;
            }

            //Enable the handbrake for the rear wheels only.
            wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
            wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
            //Enable steering for the front wheels only.
            wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = physx::PxPi * 0.3333f;
            wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = physx::PxPi * 0.3333f;
        }

        //Set up the tires.
        physx::PxVehicleTireData tires[NUMBER_OF_WHEELS];
        {
            //Set up the tires.
            for (physx::PxU32 i = 0; i < numWheels; i++)
                tires[i].mType = physics::vehicle::TireType::TIRE_TYPE_NORMAL;
        }

        //Set up the suspensions
        physx:: PxVehicleSuspensionData suspensions[NUMBER_OF_WHEELS];
        {
            //Compute the mass supported by each suspension spring.
            physx::PxF32 suspSprungMasses[NUMBER_OF_WHEELS];
            PxVehicleComputeSprungMasses
                    (numWheels, wheelCenterActorOffsets,
                     vehicleDesc.chassisCMOffset, vehicleDesc.chassisMass, 1, suspSprungMasses);

            //Set the suspension data.
            for (physx::PxU32 i = 0; i < numWheels; i++)
            {
                suspensions[i].mMaxCompression = dbgInfo.maxCompression;
                suspensions[i].mMaxDroop = dbgInfo.maxDroop;
                suspensions[i].mSpringStrength = dbgInfo.springStrength;
                suspensions[i].mSpringDamperRate = dbgInfo.springDamperRate;
                suspensions[i].mSprungMass = dbgInfo.sprungMass;
            }

            //Set the camber angles.
            const physx::PxF32 camberAngleAtRest = dbgInfo.camberAtRest;
            const physx::PxF32 camberAngleAtMaxDroop = dbgInfo.camberAtMaxDroop;
            const physx::PxF32 camberAngleAtMaxCompression = dbgInfo.maxCompression;
            for (physx::PxU32 i = 0; i < numWheels; i += 2)
            {
                suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
                suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
                suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
                suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
                suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
                suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
            }
        }

        //Set up the wheel geometry.
        physx::PxVec3 suspTravelDirections[NUMBER_OF_WHEELS];
        physx::PxVec3 wheelCentreCMOffsets[NUMBER_OF_WHEELS];
        physx::PxVec3 suspForceAppCMOffsets[NUMBER_OF_WHEELS];
        physx::PxVec3 tireForceAppCMOffsets[NUMBER_OF_WHEELS];
        {
            //Set the geometry data.
            for (physx::PxU32 i = 0; i < numWheels; i++)
            {
                //Vertical suspension travel.
                suspTravelDirections[i] = physx::PxVec3(0, -1, 0);

                //Wheel center offset is offset from rigid body center of mass.
                wheelCentreCMOffsets[i] =
                        wheelCenterActorOffsets[i] - vehicleDesc.chassisCMOffset;

                //Suspension force application point 0.3 metres below
                //rigid body center of mass.
                suspForceAppCMOffsets[i] =
                        physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

                //Tire force application point 0.3 metres below
                //rigid body center of mass.
                tireForceAppCMOffsets[i] =
                        physx::PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
            }
        }

        //Set up the filter data of the raycast that will be issued by each suspension.
        {
            physx::PxFilterData qryFilterData;
            qryFilterData.word3 = physics::vehicle::DRIVABLE_SURFACE;

            //Set the wheel, tire and suspension data.
            //Set the geometry data.
            //Set the query filter data
            for (physx::PxU32 i = 0; i < numWheels; i++)
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

            //Add a front and rear anti-roll bar
            physx::PxVehicleAntiRollBarData barFront;
            barFront.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
            barFront.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
            barFront.mStiffness = 10000.0f;
            wheelsSimData->addAntiRollBarData(barFront);
            physx::PxVehicleAntiRollBarData barRear;
            barRear.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT;
            barRear.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
            barRear.mStiffness = 10000.0f;
            wheelsSimData->addAntiRollBarData(barRear);
        }
    }

    //Set up the sim data for the vehicle drive model.
    physx::PxVehicleDriveSimData4W driveSimData;
    {
        //Diff
        physx::PxVehicleDifferential4WData diff;
        diff.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
        driveSimData.setDiffData(diff);

        //Engine
        physx::PxVehicleEngineData engine;
        engine.mPeakTorque = dbgInfo.enginPeakTorq;
        //approx 6000 rpm
        engine.mMaxOmega = dbgInfo.maxOmega;
        driveSimData.setEngineData(engine);

        //Gears
        physx::PxVehicleGearsData gears;
        gears.mSwitchTime = dbgInfo.gearSwitchTime;
        driveSimData.setGearsData(gears);

        //Clutch
        physx::PxVehicleClutchData clutch;
        clutch.mStrength = dbgInfo.clutchStrength;
        driveSimData.setClutchData(clutch);

        //Ackermann steer accuracy
        physx::PxVehicleAckermannGeometryData ackermann;
        ackermann.mAccuracy = dbgInfo.ackermannAcuracy;
        ackermann.mAxleSeparation =
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
        ackermann.mFrontWidth =
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
        ackermann.mRearWidth =
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
                wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
        driveSimData.setAckermannGeometryData(ackermann);
    }

    //Create a vehicle from the wheels and drive sim data.
    physx::PxVehicleDrive4W* vehDrive4W = physx::PxVehicleDrive4W::allocate(numWheels);
    vehDrive4W->setup(pxPhysics, veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);

    //Free the sim data because we don't need that any more.
    wheelsSimData->free();

    vehicle4W = vehDrive4W;
    vehicle = vehicle4W;

    configureUserData();

    transform = glm::translate(transform,
                               glm::vec3(0, vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f + 2.5f,0.f));
    vehicle4W->getRigidDynamicActor()->setGlobalPose(glmToPhysx(transform));
    pxScene->addActor(*vehicle4W->getRigidDynamicActor());

    //Set the vehicle to rest in first gear.
    //Set the vehicle to use auto-gears.
    vehicle4W->setToRestState();
    vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eNEUTRAL);
    vehicle4W->mDriveDynData.setUseAutoGears(true);

}

void engine::physics::components::CVehicle4Wheeled::updateDebugInfo(vehicle::DebugRaycastVehicle4W& dbgInfo)
{
    const physx::PxVehicleDriveDynData* dynData = &vehicle->mDriveDynData;
    const physx::PxVehicleWheelsSimData* wheelsSimData = &vehicle->mWheelsSimData;
    const physx::PxVehicleSuspensionData suspensionData = wheelsSimData->getSuspensionData(0);

    const physx::PxTransform currTransform = vehicle->getRigidDynamicActor()->getGlobalPose();
    const auto mat4 = physx::PxMat44(currTransform);

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
    dbgInfo.camberAtRest = suspensionData.mCamberAtRest;
    dbgInfo.camberAtMaxDroop = suspensionData.mCamberAtMaxDroop;
    dbgInfo.camberAtMaxCompression = suspensionData.mCamberAtMaxCompression;

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
    dbgInfo.bIsBraking = vehicleInputData.getDigitalBrake();
}