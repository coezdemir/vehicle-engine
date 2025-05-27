//
// Created by Cem on 13.04.2022.
//

#include <engine/physics/world/ARaycastVehicle.h>

engine::physics::world::ARaycastVehicle::ARaycastVehicle(glm::mat4 transform, engine::world::World* world)
        : AActor(world)
{

    // Init transform component
    transformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    // Init raycast vehicle component
    raycastVehicleComp = rootEntity.addComponent<physics::components::CRaycastVehicle>();
    physics::components::CRaycastVehicle::ConstructionInfo raycastVehicleInfo;
    raycastVehicleInfo.instantiator = this;
    raycastVehicleInfo.boxHalfExtends = glm::vec3(1.5f, 0.5f, 1.5f);
    raycastVehicleInfo.chassisMass = 10.f;
    raycastVehicleInfo.suspensionLength = 0.60f;
    raycastVehicleInfo.suspensionStrength = 10000.0f;
    raycastVehicleInfo.suspensionDampening = 100.f;
    raycastVehicleInfo.centerOfMassOffset = glm::vec3(0.f, 0.f, 0.f);
    raycastVehicleInfo.accelerationFactor = 25000.f;
    raycastVehicleInfo.forwardVsSteer[0] = 60.f;
    raycastVehicleInfo.forwardVsSteer[1] = 250.f;
    raycastVehicleInfo.friction = 25000.f;
    raycastVehicleComp->init(&raycastVehicleInfo);

    transformComp->event_visited.subscribe([this](scene::TreeTraverser* traverser){
        raycastVehicleComp->visit(traverser);
    });

    handle_keyInput =
            world->event_keyInput.subscribe([this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
                raycastVehicleComp->processKeyInput(key, action);
            });

    handle_controllerInput =
            world->event_controllerInput.subscribe([this](int controllerID, GLFWgamepadstate controllerState) {
                raycastVehicleComp->processControllerInput(controllerState);
            });
}

void engine::physics::world::ARaycastVehicle::tick(const float deltaTime)
{
    AActor::tick(deltaTime);

    raycastVehicleComp->tick(deltaTime);
}

const engine::physics::components::raycastVehicle_handle_t&
engine::physics::world::ARaycastVehicle::getRaycastVehicleComp() const
{
    return raycastVehicleComp;
}

glm::vec4 engine::physics::world::ARaycastVehicle::getLocation()
{
    if (transformComp->getId() == engine::components::invalid_id)
        return glm::vec4(0.f);

    return getRootEntity().getComponent<engine::scene::components::CTransform>(
            transformComp->getId())->getTransform()[3];
}
