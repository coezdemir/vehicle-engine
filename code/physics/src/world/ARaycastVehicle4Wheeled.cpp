//
// Created by CemOe on 30.09.2021.
//

#include <engine/physics/world/ARaycastVehicle4Wheeled.h>
#include <engine/renderer/Renderer.h>

engine::physics::world::ARaycastVehicle4Wheeled::ARaycastVehicle4Wheeled(glm::mat4 transform, engine::world::World* world)
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
    raycastVehicleComp = rootEntity.addComponent<physics::components::CVehicle4Wheeled>();
    physics::components::CVehicle4Wheeled::ConstructionInfo raycastVehicleInfo;
    raycastVehicleInfo.instantiator = this;
    raycastVehicleInfo.vehicleDesc = vehicleDesc;
    raycastVehicleComp->init(&raycastVehicleInfo);

    transformComp->event_visited.subscribe([this](scene::TreeTraverser* traverser) {
        raycastVehicleComp->visit(traverser);
    });

    handle_keyInput =
            world->event_keyInput.subscribe([this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
                raycastVehicleComp->processInput(key, action);
            });

    handle_controllerInput =
            world->event_controllerInput.subscribe([this](int controllerID, GLFWgamepadstate controllerState) {
                raycastVehicleComp->processInput(controllerState);
            });
}

void engine::physics::world::ARaycastVehicle4Wheeled::tick(const float deltaTime)
{
    AActor::tick(deltaTime);

    raycastVehicleComp->tick(deltaTime);
}

const engine::physics::components::raycastWheeledVehicle_handle_t&
engine::physics::world::ARaycastVehicle4Wheeled::getRaycastVehicleComp() const
{
    return raycastVehicleComp;
}

engine::physics::vehicle::VehicleDesc engine::physics::world::ARaycastVehicle4Wheeled::initVehicleDesc()
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
    physics::vehicle::VehicleDesc vehicleDesc;
    physx::PxVec3 chassisDims = physx::PxVec3(1.1f, 1.f, 2.5f);
    vehicleDesc.chassisMass = 750.f;
    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = physx::PxVec3((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * vehicleDesc.chassisMass / 12.0f,
                                           (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * vehicleDesc.chassisMass / 12.0f,
                                           (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * vehicleDesc.chassisMass / 12.0f);
    vehicleDesc.chassisCMOffset = physx::PxVec3(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);
    vehicleDesc.chassisSimFilterData = physx::PxFilterData(physics::vehicle::COLLISION_FLAG_CHASSIS,
                                                           physics::vehicle::COLLISION_FLAG_CHASSIS_AGAINST,
                                                           0,
                                                           0);

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    vehicleDesc.wheelMass = 20.f;
    vehicleDesc.wheelRadius = 0.3f;
    vehicleDesc.wheelWidth = 0.4f;
    vehicleDesc.wheelMOI = 0.5f * vehicleDesc.wheelMass * vehicleDesc.wheelRadius * vehicleDesc.wheelRadius;
    vehicleDesc.numWheels = 4;
    vehicleDesc.chassisSimFilterData = physx::PxFilterData(physics::vehicle::COLLISION_FLAG_WHEEL,
                                                           physics::vehicle::COLLISION_FLAG_WHEEL_AGAINST,
                                                           0,
                                                           0);

    vehicleDesc.actorUserData = nullptr;
    vehicleDesc.shapeUserDatas = nullptr;

    return vehicleDesc;
}

void engine::physics::world::ARaycastVehicle4Wheeled::debugVehicle(engine::physics::vehicle::DebugRaycastVehicle4W& debugInfo)
{
    raycastVehicleComp->debugVehicle(debugInfo);
}

void engine::physics::world::ARaycastVehicle4Wheeled::updateDebugInfo(vehicle::DebugRaycastVehicle4W& dbgInfo)
{
    raycastVehicleComp->updateDebugInfo(dbgInfo);
}

glm::vec4 engine::physics::world::ARaycastVehicle4Wheeled::getLocation()
{
    if (transformComp->getId() == engine::components::invalid_id)
        return glm::vec4(0.f);

    return getRootEntity().getComponent<engine::scene::components::CTransform>(
            transformComp->getId())->getTransform()[3];
}