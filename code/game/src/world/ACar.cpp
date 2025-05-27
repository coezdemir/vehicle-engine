//
// Created by coezdemir on 21.11.2020.
//
#include "ACar.h"
#include <utility>
#include "engine/renderer/world/ACamera.h"
#include "../physics/MyCollisionCallback.h"
#include <engine/physics/Physics.h>
#include <GLFW/glfw3.h>

const std::pair<float, float> SPEED_RANGE = {0.f, 50.f};
const std::pair<float, float> CAMERA_OFFSET_RANGE = {-18.f, -28.f};

static inline float
mapInputToRange(float value, std::pair<float, float> inputRange, std::pair<float, float> outputRange)
{
    return outputRange.first +
           (
                   (outputRange.second - outputRange.first) / (inputRange.second - inputRange.first) *
                   (value - inputRange.first)
           );
}

game::world::ACar::ACar(const glm::mat4 transform, engine::world::World* const world,
                        const engine::physics::PhysicsSystem* physicsSystem)
        : ARaycastVehicle(transform, world),
          pxScene(physicsSystem->getPxScene()), lastCheckpoint(nullptr), startTransform(transform), state(NORMAL),
          currentItem(NO_ITEM), itemActiveTime(0.f), id(-1), lastCollidedCar(nullptr),
          currentRestTimeCollidedCar(RESET_TIME_LAST_COLLIDED_CAR), bCollided(false), bFlippedOver(false)
{
    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "game/spacecraft/player1/space-vehicle.obj";
    meshComp->init(&meshInfo);

    sphereCollisionComp = rootEntity.addComponent<engine::physics::components::CSphereCollision>();
    engine::physics::components::CSphereCollision::ConstructionInfo sphereCollisionInfo;
    sphereCollisionInfo.instantiator = this;
    sphereCollisionInfo.bIsDynamic = false;
    sphereCollisionInfo.bIsSimulationShape = false;
    sphereCollisionInfo.sphereRadius = 15.f;
    sphereCollisionInfo.bShouldDeactivateAfterCollision = false;
    sphereCollisionInfo.bUpdateLocation = true;
    sphereCollisionInfo.bIsSimulationShape = false;
    sphereCollisionInfo.timeActive = ENGINE_OFF_TRIGGER_ACTIVE_TIME;
    sphereCollisionInfo.bActiveFromStart = false;
    sphereCollisionComp->init(&sphereCollisionInfo);
    sphereCollisionComp->event_onOverlap.subscribe(
            [this](const engine::physics::components::CollisionInfo& collisionInfo) {
                handleEngineOffCollision(collisionInfo);
            }
    );

    glm::mat4 camTransform = glm::mat4(1.f);
    camTransform = glm::translate(camTransform, glm::vec3(0.f, 5.f, -18.f));
    camTransformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = camTransform;
    camTransformComp->init(&transformInfo);
    transformComp->addChild(camTransformComp->getId());

    cameraComp = rootEntity.addComponent<engine::renderer::components::CCamera>();
    engine::renderer::components::CCamera::ConstructionInfo constructionInfo =
            engine::renderer::components::CCamera::ConstructionInfo();
    constructionInfo.instantiator = this;
    constructionInfo.targetLoc = [this]() { return transformComp->getTransform()[3]; };
    constructionInfo.cameraType = engine::renderer::components::CT_PERSPECTIVE;
    cameraComp->init(&constructionInfo);

    glm::mat4 particleTransform = glm::mat4(1.f);
    particleTransform = glm::translate(particleTransform, glm::vec3(0.f, 0.f, 0.f));
    particleTransformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    transformInfo.instantiator = this;
    transformInfo.transform = particleTransform;
    particleTransformComp->init(&transformInfo);
    transformComp->addChild(particleTransformComp->getId());

    knockbackDownParticle = rootEntity.addComponent<engine::renderer::components::CParticleEmitter>();
    engine::renderer::components::CParticleEmitter::ConstructionInfo particleConstruction;
    particleConstruction.instantiator = this;
    engine::renderer::particle::ParticleSystemProps particleSystemProps = engine::renderer::particle::ParticleSystemProps();
    particleSystemProps.bActiveFromStart = false;
    particleSystemProps.numberOfParticles = 8;
    particleSystemProps.offsetX = {-2.f, 2.f};
    particleSystemProps.offsetY = {0.3f, 1.f};
    particleSystemProps.offsetZ = {-3.f, 3.f};
    particleSystemProps.sizeOverLife = {glm::vec3(1.f), glm::vec3(1.f)};
    particleSystemProps.velocityMinMax = {glm::vec3(0.f, -0.5f, 0.f), glm::vec3(0.f, -0.5f, 0.f)};
    particleSystemProps.colorOverLife = {glm::vec4(1.f, 0.f, 0.f, 1.f), glm::vec4(1.f, 0.f, 0.f, 1.f)};
    particleSystemProps.particleLifetime = 3.f;
    particleSystemProps.bRecycleOldest = false;
    particleSystemProps.bUsesModel = true;
    particleSystemProps.modelPath = "game/powerUpDown/power-down.obj";
    particleConstruction.particleSystemProps = &particleSystemProps;
    knockbackDownParticle->init(&particleConstruction);

    knockbackUpParticle = rootEntity.addComponent<engine::renderer::components::CParticleEmitter>();
    particleConstruction.instantiator = this;
    particleSystemProps.offsetY = {-1.f, 0.f};
    particleSystemProps.velocityMinMax = {glm::vec3(0.f, 0.5f, 0.f), glm::vec3(0.f, 0.5f, 0.f)};
    particleSystemProps.colorOverLife = {glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec4(0.f, 0.f, 1.f, 1.f)};
    particleSystemProps.modelPath = "game/powerUpDown/power-up.obj";
    particleConstruction.particleSystemProps = &particleSystemProps;
    knockbackUpParticle->init(&particleConstruction);

    glm::mat4 manipulationTransform = glm::mat4(1.f);
    manipulationTransform = glm::scale(manipulationTransform, glm::vec3(3.f, 3.f, 3.f));
    manipulationTransformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    transformInfo.instantiator = this;
    transformInfo.transform = manipulationTransform;
    manipulationTransformComp->init(&transformInfo);
    transformComp->addChild(manipulationTransformComp->getId());

    meshManipulation = rootEntity.addComponent<engine::renderer::components::CModel>();
    meshInfo.instantiator = this;
    meshInfo.path = "game/manipulation/manipulation.obj";
    meshManipulation->init(&meshInfo);
    meshManipulation->setVisibility(false);

    ghostMesh = rootEntity.addComponent<engine::renderer::components::CModel>();
    meshInfo.instantiator = this;
    meshInfo.path = "game/ghost-vehicle/ghost-vehicle.obj";
    ghostMesh->init(&meshInfo);
    ghostMesh->setVisibility(false);

    scaleComp = rootEntity.addComponent<engine::scene::components::CScale>();
    engine::scene::components::CScale::ConstructionInfo scaleInfo;
    scaleInfo.instantiator = this;
    scaleInfo.bActiveFromStart = false;
    scaleInfo.transformComp = manipulationTransformComp;
    scaleInfo.scaleBetween = {glm::vec3(1.f), glm::vec3(4.f)};
    scaleInfo.speed = 3.f;
    scaleInfo.bShouldReverse = false;
    scaleComp->init(&scaleInfo);

    crashSound = rootEntity.addComponent<engine::audio::components::CAudio>();
    engine::audio::components::CAudio::ConstructionInfo audioConstruction;
    audioConstruction.instantiator = this;
    audioConstruction.pathToWav = "sound-effects/bounce2.wav";
    audioConstruction.volume = 2.f;
    crashSound->init(&audioConstruction);

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        raycastVehicleComp->visit(traverser);
        meshComp->visit(traverser);
        ghostMesh->visit(traverser);
        sphereCollisionComp->visit(traverser);
    });

    camTransformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser) {
        cameraComp->initialVisit(traverser);
    });

    camTransformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        cameraComp->visit(traverser);
    });

    particleTransformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        knockbackDownParticle->visit(traverser);
        knockbackUpParticle->visit(traverser);
    });

    manipulationTransformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshManipulation->visit(traverser);
    });

    raycastVehicleComp->event_onPhysicsStep.subscribe([this]() {
        if (collided())
        {
            raycastVehicleComp->clearAllForces();
            raycastVehicleComp->addForce(force, physx::PxForceMode::eVELOCITY_CHANGE);
            raycastVehicleComp->clearTorque(physx::PxForceMode::eFORCE);
            raycastVehicleComp->clearTorque(physx::PxForceMode::eIMPULSE);
            crashSound->play();
            bCollided = false;
        }
    });

    raycastVehicleComp->event_onRigidBodyCreated.subscribe([](const physx::PxRigidActor* rigidbody) {
        game::physics::collision::FilterGroup::setupFiltering(rigidbody,
                                                              game::physics::collision::FilterGroup::eVEHICLE,
                                                              game::physics::collision::FilterGroup::eOBSTACLE);
    });

    world->event_keyInput.unsubscribe(handle_keyInput);
    world->event_controllerInput.unsubscribe(handle_controllerInput);

    handle_keyInput =
            world->event_keyInput.subscribe(
                    [this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
                        keyInput(key, scancode, action, mods);
                    });

    handle_controllerInput =
            world->event_controllerInput.subscribe([this](int controllerID, GLFWgamepadstate controllerState) {
                controllerInput(controllerID, controllerState);
            });
}

void game::world::ACar::tick(const float deltaTime)
{
    ARaycastVehicle::tick(deltaTime);

    if (state != NORMAL || itemActiveTime > 0.f)
    {
        itemActiveTime -= deltaTime;
        if (itemActiveTime < 0)
            resetItemState();
    }

    if (raycastVehicleComp->isInAir())
    {
        // TODO: Change to center of mass to let the Acar flip over on its own
        if (checkIfFlippedOver())
            respawn(true);
    }

    float camOffset = mapInputToRange(raycastVehicleComp->getForwardSpeed(), SPEED_RANGE, CAMERA_OFFSET_RANGE);
    glm::mat4 camTransform = glm::mat4(1.f);
    camTransform = glm::translate(camTransform, glm::vec3(0.f, 5.f, camOffset));
    camTransformComp->setLocalTransform(camTransform);

    knockbackUpParticle->tick(deltaTime);
    knockbackDownParticle->tick(deltaTime);
    scaleComp->tick(deltaTime);

}

void game::world::ACar::activateItem()
{
    if (state == NORMAL)
    {
        switch (currentItem)
        {
            case MANIPULATION:
                activateEngineOffItem();
                break;
            case GHOST:
                game::physics::collision::FilterGroup::setupFiltering(raycastVehicleComp->getRigidBody(),
                                                                      game::physics::collision::FilterGroup::eGHOST,
                                                                      game::physics::collision::FilterGroup::eOBSTACLE);
                meshComp->setVisibility(false);
                ghostMesh->setVisibility(true);
                break;
            default:
                applyItemEffect(currentItem);
                break;
        }
        currentItem = NO_ITEM;
    }
}

void game::world::ACar::applyItemEffect(const EItem item)
{
    switch (item)
    {
        case KNOCKBACK_UP:
            state = KNOCKBACK_INCREASE;
            itemActiveTime = 10.f;
            knockbackUpParticle->activate();
            break;
        case KNOCKBACK_DOWN:
            state = KNOCKBACK_DECREASE;
            itemActiveTime = 10.f;
            knockbackDownParticle->activate();
            break;
        case GHOST:
            state = NO_COLLISION;
            itemActiveTime = 5.f;
            break;
        case MANIPULATION:
            state = ENGINE_OFF;
            itemActiveTime = 5.f;
            break;
        default:
            break;
    }
}


void game::world::ACar::activateEngineOffItem()
{
    sphereCollisionComp->activate();
    itemActiveTime = ENGINE_OFF_TRIGGER_ACTIVE_TIME;
    meshManipulation->setVisibility(true);
    scaleComp->activate();
}

void game::world::ACar::keyInput(const int key, const int scanecode, const int action, const int mods)
{
    if (state == ENGINE_OFF)
        return;

    if (!glfwJoystickPresent(id))
    {
        if ((id == 0 && !glfwJoystickPresent(GLFW_JOYSTICK_1)) || (id == 1 && !glfwJoystickPresent(GLFW_JOYSTICK_2)))
        {
            if (id == 0)
            {
                raycastVehicleComp->processKeyInput(key, action);
            } else
            {
                switch (key)
                {
                    case GLFW_KEY_UP:
                        raycastVehicleComp->handleForwardInput(action);
                        break;
                    case GLFW_KEY_DOWN:
                        raycastVehicleComp->handleBackwardInput(action);
                        break;
                    case GLFW_KEY_LEFT:
                        raycastVehicleComp->handleSteerLeftInput(action);
                        break;
                    case GLFW_KEY_RIGHT:
                        raycastVehicleComp->handleSteerRightInput(action);
                        break;
                    default:
                        return;
                }
            }
        }
        else if (glfwJoystickPresent(GLFW_JOYSTICK_1) || glfwJoystickPresent(GLFW_JOYSTICK_2))
        {
            if (id == 2)
            {
                raycastVehicleComp->processKeyInput(key, action);
            } else
            {
                switch (key)
                {
                    case GLFW_KEY_UP:
                        raycastVehicleComp->handleForwardInput(action);
                        break;
                    case GLFW_KEY_DOWN:
                        raycastVehicleComp->handleBackwardInput(action);
                        break;
                    case GLFW_KEY_LEFT:
                        raycastVehicleComp->handleSteerLeftInput(action);
                        break;
                    case GLFW_KEY_RIGHT:
                        raycastVehicleComp->handleSteerRightInput(action);
                        break;
                    default:
                        return;
                }
            }
        }

        if (id == 0 || id == 2)
        {
            if (key == GLFW_KEY_E && action == GLFW_PRESS)
                activateItem();

            if (id == 0)
            {
                if (key == GLFW_KEY_P)
                    raycastVehicleComp->printLoc();
            }
        }
        if (id == 1 || id == 3)
        {
            if (key == GLFW_KEY_KP_0 && action == GLFW_PRESS)
                activateItem();
        }
    }
}

void game::world::ACar::respawn(const bool bDueToFlip)
{
    bFlippedOver = false;

    if (!lastCheckpoint)
    {
        reset()();

        if (!bDueToFlip)
            event_respawned.fire(lastCollidedCar);

        return;
    }

    glm::mat4 respawnTransform = lastCheckpoint->getRespawnTransform(id);
    raycastVehicleComp->setResetTransform(glmToPhysx(respawnTransform));
    raycastVehicleComp->reset();
    resetItemState();

    if (!bDueToFlip)
        event_respawned.fire(lastCollidedCar);
}

void game::world::ACar::controllerInput(const int controllerID, const GLFWgamepadstate controllerState)
{
    if (state == ENGINE_OFF)
        return;

    if (controllerID == id)
    {
        raycastVehicleComp->processControllerInput(controllerState);

        if (controllerState.buttons[GLFW_GAMEPAD_BUTTON_Y])
            activateItem();
    }
}


std::function<void()> game::world::ACar::reset()
{
    // Remove from physics world
    raycastVehicleComp->deactivate();

    return [this]() {
        PxTransform resetTransform = glmToPhysx(startTransform);
        raycastVehicleComp->setResetTransform(resetTransform);
        lastCheckpoint = nullptr;
        raycastVehicleComp->reset();
        resetItemState();
    };
}

bool game::world::ACar::checkIfFlippedOver()
{
    PxVec3 origin = raycastVehicleComp->getRigidBody()->getGlobalPose().p;
    origin -= PxVec3(0.f, raycastVehicleComp->getBoxHalfExtends().y, 0.f);
    PxVec3 direction = glmToPhysx(transformComp->getLocalTransform()).rotateInv(PxVec3(0.f, 1.f, 0.f));
    direction.normalize();
    PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;
    PxQueryFilterData filterData(PxQueryFlag::eSTATIC);
    float length = raycastVehicleComp->getBoxHalfExtends().y;
    physx::PxRaycastBuffer hit;

    if (pxScene->raycast(origin, direction, length, hit, hitFlags, filterData))
    {
        bFlippedOver = true;
        return true;
    }

    return false;
}

void game::world::ACar::playCrashSound()
{
    crashSound->play();
}

bool game::world::ACar::collided() const
{
    return bCollided;
}

void game::world::ACar::handleEngineOffCollision(const engine::physics::components::CollisionInfo& collisionInfo)
{
    if (ACar* otherCar = dynamic_cast<ACar*>(collisionInfo.otherActor))
    {
        if (otherCar == this)
            return;

        otherCar->applyItemEffect(MANIPULATION);
    }
}

void game::world::ACar::setCollided(const bool value, const physx::PxVec3& forceVec, ACar* const otherCar)
{
    bCollided = value;
    force = forceVec;
    setLastCollidedCar(otherCar);
}

void game::world::ACar::setStartTransform(const glm::mat4& transform)
{
    ACar::startTransform = transform;
}

void game::world::ACar::setLastCollidedCar(ACar* const collidedCar)
{
    ACar::lastCollidedCar = collidedCar;
}

void game::world::ACar::setID(const int newID)
{
    id = newID;

    raycastVehicleComp->setUseControllerInput(glfwJoystickPresent(id));

    switch (id)
    {
        case 1:
            meshComp->setModel("game/spacecraft/player2/space-vehicle.obj");
            break;
        case 2:
            meshComp->setModel("game/spacecraft/player3/space-vehicle.obj");
            break;
        case 3:
            meshComp->setModel("game/spacecraft/player4/space-vehicle.obj");
            break;
    }
}

void game::world::ACar::setCurrentItem(const EItem newItem)
{
    currentItem = newItem;
    std::cout << "Got item: ";
    std::cout << currentItem << std::endl;
}

void game::world::ACar::setCheckpoint(const game::world::ACheckpoint* const checkpoint)
{
    lastCheckpoint = checkpoint;
}

ECarState game::world::ACar::getState() const
{
    return state;
}

const game::world::ACheckpoint* game::world::ACar::getLastCheckpoint() const
{
    return lastCheckpoint;
}

int game::world::ACar::getId() const
{
    return id;
}

EItem game::world::ACar::getCurrentItem() const
{
    return currentItem;
}

void game::world::ACar::resetItemState()
{
    meshComp->setVisibility(true);
    ghostMesh->setVisibility(false);
    knockbackUpParticle->deactivate();
    knockbackDownParticle->deactivate();
    sphereCollisionComp->deactivate();
    meshManipulation->setVisibility(false);
    scaleComp->deactivate();
    scaleComp->reset();
    game::physics::collision::FilterGroup::setupFiltering(raycastVehicleComp->getRigidBody(),
                                                          game::physics::collision::FilterGroup::eVEHICLE,
                                                          game::physics::collision::FilterGroup::eOBSTACLE);

    itemActiveTime = 0;
    state = NORMAL;
}
