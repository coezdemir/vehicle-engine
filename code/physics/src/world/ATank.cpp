//
// Created by Cem on 15.06.2022.
//

#include "engine/physics/world/ATank.h"

engine::physics::world::ATank::ATank(glm::mat4 transform, engine::world::World* world)
        : AActor(world)
{
    // Init transform component
    transformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    physics::vehicle::VehicleDesc vehicleDesc = initVehicleDesc();

    // Init raycast vehicle component
    tankComp = rootEntity.addComponent<physics::components::CVehicleTank>();
    physics::components::CVehicleTank::ConstructionInfo tankInfo;
    tankInfo.instantiator = this;
    tankInfo.vehicleDesc = vehicleDesc;
    tankComp->init(&tankInfo);

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        tankComp->visit(traverser);
    });

    handle_keyInput =
            world->event_keyInput.subscribe([this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
                tankComp->processInput(key, action);
            });

    handle_controllerInput =
            world->event_controllerInput.subscribe([this](int controllerID, GLFWgamepadstate controllerState) {
                tankComp->processInput(controllerState);
            });

}

void engine::physics::world::ATank::tick(float deltaTime)
{
    AActor::tick(deltaTime);

    tankComp->tick(deltaTime);
}

engine::physics::vehicle::VehicleDesc engine::physics::world::ATank::initVehicleDesc()
{
    engine::physics::vehicle::VehicleDesc vehicleDesc;
    const physx::PxF32 chassisMass = 1500.0f;
    const physx::PxVec3 chassisDims = physx::PxVec3(3.5f, 2.f, 9.f);
    const physx::PxVec3 chassisMOI =
            physx::PxVec3((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
                          (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
                          (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
    const physx::PxVec3 chassisCMOffset = physx::PxVec3(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    const physx::PxF32 wheelMass = 20.0f;
    const physx::PxF32 wheelRadius = 0.5f;
    const physx::PxF32 wheelWidth = 0.4f;
    const physx::PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
    const physx::PxU32 nbWheels = 14;

    vehicleDesc.chassisMass = chassisMass;
    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisSimFilterData = physx::PxFilterData(physics::vehicle::COLLISION_FLAG_CHASSIS,
                                                           physics::vehicle::COLLISION_FLAG_CHASSIS_AGAINST,
                                                           0,
                                                           0);

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    vehicleDesc.wheelMass = wheelMass;
    vehicleDesc.wheelRadius = wheelRadius;
    vehicleDesc.wheelWidth = wheelWidth;
    vehicleDesc.wheelMOI = wheelMOI;
    vehicleDesc.numWheels = nbWheels;
    vehicleDesc.chassisSimFilterData = physx::PxFilterData(physics::vehicle::COLLISION_FLAG_WHEEL,
                                                           physics::vehicle::COLLISION_FLAG_WHEEL_AGAINST,
                                                           0,
                                                           0);

    vehicleDesc.actorUserData = nullptr;
    vehicleDesc.shapeUserDatas = nullptr;
    vehicleDesc.chassisMaterial = nullptr;
    vehicleDesc.wheelMaterial = nullptr;

    return vehicleDesc;
}

const engine::physics::components::vehicleTank_handle_t& engine::physics::world::ATank::getTankComp() const
{
    return tankComp;
}

void engine::physics::world::ATank::debugVehicle(engine::physics::vehicle::DebugVehicleTank& debugInfo)
{
    tankComp->debugVehicle(debugInfo);
}

void engine::physics::world::ATank::updateDebugInfo(vehicle::DebugVehicleTank& dbgInfo)
{
    tankComp->updateDebugInfo(dbgInfo);
}