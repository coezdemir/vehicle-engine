//
// Created by Cem on 13.04.2022.
//

#include "engine/physics/components/vehicles/CRaycastVehicle.h"
#include "engine/scene/TreeTraverser.h"
#include "engine/physics/Physics.h"

using namespace physx;

engine::physics::components::CRaycastVehicle::CRaycastVehicle(const engine::components::id_t id,
                                                              const engine::entities::id_t owner)
        : CPhysicsComponent(id, owner)
{
    bIsActive = false;
    bIsInAir = false;
    bUseControllerInput = false;
    rigidDynamic = nullptr;
}

void
engine::physics::components::CRaycastVehicle::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);
    // Check if we have the right construction info
    auto* raycastVehicleConstruction =
            dynamic_cast<const engine::physics::components::CRaycastVehicle::ConstructionInfo*>(constructionInfo);
    assert(raycastVehicleConstruction &&
                   "Please use the correct ConstructionInfo: RaycastVehicleComponent::ConstructionInfo");

    boxHalfExtends = raycastVehicleConstruction->boxHalfExtends;
    suspensionLength = raycastVehicleConstruction->suspensionLength;
    suspensionStrength = raycastVehicleConstruction->suspensionStrength;
    chassisMass = raycastVehicleConstruction->chassisMass;
    suspensionDampening = raycastVehicleConstruction->suspensionDampening;
    centerOfMassOffset = raycastVehicleConstruction->centerOfMassOffset;
    accelerationFactor = raycastVehicleConstruction->accelerationFactor;
    friction = raycastVehicleConstruction->friction;

    for (int i = 0; i < FORWARD_STEER_PAIRS * 2; ++i)
        forwardVsSteer[i] = raycastVehicleConstruction->forwardVsSteer[i];
}

void engine::physics::components::CRaycastVehicle::tick(const float deltaTime)
{
    CComponent::tick(deltaTime);

    if(bIsActive)
    {
        evaluateInput(deltaTime);

        applyTraction(deltaTime);
    }
}

void engine::physics::components::CRaycastVehicle::stepPhysics(const float stepSize)
{
    if(!bIsActive)
        return;

    fireVoidEvent(&event_onPhysicsStep);

    PxVec3 frontRight = localTransform.transform(PxVec3(boxHalfExtends.x, -boxHalfExtends.y, boxHalfExtends.z));
    PxVec3 frontLeft = localTransform.transform(PxVec3(-boxHalfExtends.x, -boxHalfExtends.y, boxHalfExtends.z));
    PxVec3 backRight = localTransform.transform(PxVec3(boxHalfExtends.x, -boxHalfExtends.y, -boxHalfExtends.z));
    PxVec3 backLeft = localTransform.transform(PxVec3(-boxHalfExtends.x, -boxHalfExtends.y, -boxHalfExtends.z));
    PxVec3 origins[]{frontRight, frontLeft, backRight, backLeft};
    PxVec3 direction = localTransform.rotate(PxVec3(0.f, -1.f, 0.f));
    direction.normalize();
    PxHitFlags hitFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;
    PxQueryFilterData filterData(PxQueryFlag::eSTATIC);

    int hitting = 0;
    float compression, force;
    PxVec3 forceVec, velocityAtPoint;
    for (const PxVec3& origin: origins)
    {
        if (pxScene->raycast(origin, direction, suspensionLength, hit, hitFlags, filterData))
        {
            compression = 1 - hit.block.distance / suspensionLength;
            velocityAtPoint = PxRigidBodyExt::getVelocityAtPos(*rigidDynamic, origin);
            force = suspensionStrength * compression - suspensionDampening * velocityAtPoint.y;
            if (force > 0)
            {
                forceVec = -direction * force;
                PxRigidBodyExt::addForceAtPos(*rigidDynamic, forceVec * stepSize, origin, PxForceMode::eFORCE);
            }
            ++hitting;
        }
    }

    if (!hitting)
        bIsInAir = true;
    else
        bIsInAir = false;

    fireVoidEvent(&event_onPhysicsStepEnd);
}

void engine::physics::components::CRaycastVehicle::initPhysics()
{
    createVehicleChassis();
}

void engine::physics::components::CRaycastVehicle::createVehicleChassis()
{
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;

    decomposeTransform(transform, translation, rotation, scale);

    physx::PxVec3 pxBoxHalfExtents = glmToPhysx(boxHalfExtends);
    pxBoxHalfExtents.x *= scale.x;
    pxBoxHalfExtents.y *= scale.y;
    pxBoxHalfExtents.z *= scale.z;

    physx::PxShapeFlags shapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE |
                                     physx::PxShapeFlag::eSIMULATION_SHAPE;

    rigidBody = pxPhysics->createRigidDynamic(glmToPhysx(transform));
    rigidBody->userData = this;

    shape =
            physx::PxRigidActorExt::createExclusiveShape(*rigidBody, physx::PxBoxGeometry(pxBoxHalfExtents),
                                                         *pxDefaultMat, shapeFlags);

    fireRigidActorEvent(&event_onRigidBodyCreated);

    rigidDynamic = rigidBody->is<PxRigidDynamic>();
    rigidDynamic->setMass(chassisMass);
    rigidDynamic->setLinearDamping(0.7f);
    rigidDynamic->setAngularDamping(1.f);

    PxTransform cMassOffset = PxTransform(glmToPhysx(centerOfMassOffset));
    rigidDynamic->setCMassLocalPose(cMassOffset);
    pxScene->addActor(*rigidBody);

    bIsActive = true;

    resetTransform = rigidBody->getGlobalPose();

    localTransform = PxTransform(glmToPhysx(translation), glmToPhysx(rotation));

    event_physicsInitialized.fire();
}

void engine::physics::components::CRaycastVehicle::activate()
{
    if (!bIsActive && bInitialTraversed)
    {
        rigidBody->setGlobalPose(resetTransform);
        pxScene->addActor(*rigidBody);
        /*
            TODO: Fix concurrent API call and writes -> dunno we do not have multiple threads, but physx is complaining
             maybe mark actors for removal and remove them after the next tick
        */
        clearAllForces();
        transform = physxToGLM(resetTransform);
        bNeedsUpdate = true;
        bIsActive = true;
    }
}

void engine::physics::components::CRaycastVehicle::deactivate()
{
    if (bIsActive && bInitialTraversed)
    {
        pxScene->removeActor(*rigidBody);
        bIsActive = false;
    }
}

engine::physics::vehicle::DebugRaycastVehicle engine::physics::components::CRaycastVehicle::getDebugInfo()
{
    float linearDamp = 0.f;
    float angularDamp = 0.f;

    if (rigidDynamic)
    {
        linearDamp = rigidDynamic->getLinearDamping();
        angularDamp = rigidDynamic->getAngularDamping();
    }

    return {{boxHalfExtends.x * 2, boxHalfExtends.y * 2, boxHalfExtends.z * 2},
            chassisMass, suspensionLength, suspensionStrength, suspensionDampening,
            {centerOfMassOffset.x, centerOfMassOffset.y, centerOfMassOffset.z},
            accelerationFactor, accelerationFactor, turningRate, friction, getForwardSpeed(),
            linearDamp, angularDamp};
}

void
engine::physics::components::CRaycastVehicle::updateUsingDbgInfo(
        engine::physics::vehicle::DebugRaycastVehicle& dbgInfo)
{
    pxScene->removeActor(*rigidBody);
    rigidBody->detachShape(*shape);

    boxHalfExtends = glm::vec3(dbgInfo.chassisDims[0] / 2,
                               dbgInfo.chassisDims[1] / 2,
                               dbgInfo.chassisDims[2] / 2);
    chassisMass = dbgInfo.chassisMass;
    suspensionLength = dbgInfo.suspensionLength;
    suspensionStrength = dbgInfo.suspensionStrength;
    suspensionDampening = dbgInfo.suspensionDampening;
    centerOfMassOffset = glm::vec3(dbgInfo.centerOfMassOffset[0],
                                   dbgInfo.centerOfMassOffset[1],
                                   dbgInfo.centerOfMassOffset[2]);
    accelerationFactor = dbgInfo.accelerationFactor;
    turningRate = dbgInfo.turningRate;
    friction = dbgInfo.friction;

    physx::PxShapeFlags shapeFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE |
                                     physx::PxShapeFlag::eSIMULATION_SHAPE;
    shape =
            physx::PxRigidActorExt::createExclusiveShape(*rigidBody,
                                                         physx::PxBoxGeometry(glmToPhysx(boxHalfExtends)),
                                                         *pxDefaultMat,
                                                         shapeFlags);
    rigidBody->attachShape(*shape);

    rigidDynamic->setMass(chassisMass);
    rigidDynamic->setLinearDamping(dbgInfo.linearDamping);
    rigidDynamic->setAngularDamping(dbgInfo.angularDamping);

    PxTransform cMassOffset = PxTransform(glmToPhysx(centerOfMassOffset));
    rigidDynamic->setCMassLocalPose(cMassOffset);

    pxScene->addActor(*rigidBody);
}

void
engine::physics::components::CRaycastVehicle::initialVisit(scene::TreeTraverser* traverser)
{
    bInitialTraversed = true;
    transform = traverser->getStackTop();
    initPhysics();
}

void engine::physics::components::CRaycastVehicle::visit(scene::TreeTraverser* traverser)
{
    if (!bInitialTraversed)
    {
        initialVisit(traverser);
        return;
    }

    if (!bNeedsUpdate || !bIsActive)
        return;

    engine::scene::components::transform_handle_t transformComponent = traverser->getCurrTransformComp();

    traverser->popStack();

    transformComponent->setTransform(transform);

    glm::mat4 newLocal = glm::inverse(traverser->getStackTop()) * transform;
    transformComponent->setLocalTransform(newLocal);

    traverser->pushStack(transformComponent->getLocalTransform());
    localTransform = glmToPhysx(transformComponent->getLocalTransform());
    bNeedsUpdate = false;
}

void engine::physics::components::CRaycastVehicle::processKeyInput(int key, int action)
{
    switch (key)
    {
        case GLFW_KEY_W:
            handleForwardInput(action);
            break;
        case GLFW_KEY_S:
            handleBackwardInput(action);
            break;
        case GLFW_KEY_A:
            handleSteerLeftInput(action);
            break;
        case GLFW_KEY_D:
            handleSteerRightInput(action);
            break;
    }
}

void engine::physics::components::CRaycastVehicle::processControllerInput(GLFWgamepadstate controllerState)
{
    float steeringValue = controllerState.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    steeringValue = std::trunc(steeringValue * 10) / 10;
    controllerInput.steeringValue = steeringValue * -1;
    controllerInput.accelerationValue = controllerState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
    controllerInput.breakValue = controllerState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
}

void engine::physics::components::CRaycastVehicle::handleForwardInput(int action)
{
    if (action == GLFW_PRESS)
        keyInput.isKeyWPressedDown = true;
    else if (action == GLFW_RELEASE)
        keyInput.isKeyWPressedDown = false;
}

void engine::physics::components::CRaycastVehicle::handleBackwardInput(int action)
{
    if (action == GLFW_PRESS)
        keyInput.isKeySPressedDown = true;
    else if (action == GLFW_RELEASE)
        keyInput.isKeySPressedDown = false;
}

void engine::physics::components::CRaycastVehicle::handleSteerLeftInput(int action)
{
    if (action == GLFW_PRESS)
        keyInput.isKeyAPressedDown = true;
    else if (action == GLFW_RELEASE)
        keyInput.isKeyAPressedDown = false;
}

void engine::physics::components::CRaycastVehicle::handleSteerRightInput(int action)
{
    if (action == GLFW_PRESS)
        keyInput.isKeyDPressedDown = true;
    else if (action == GLFW_RELEASE)
        keyInput.isKeyDPressedDown = false;
}

void engine::physics::components::CRaycastVehicle::reset()
{
    deactivate();
    transform = physxToGLM(resetTransform);
    bNeedsUpdate = true;
    activate();
}

void engine::physics::components::CRaycastVehicle::applyTraction(const float deltaTime)
{
    if (!bIsInAir)
    {
        PxVec3 velocity = rigidDynamic->getLinearVelocity();
        PxVec3 sideWaysDirection = PxVec3(1.f, 0.f, 0.f);
        PxVec3 localVelocity = localTransform.rotateInv(velocity);

        float dotP = localVelocity.dot(sideWaysDirection);
        PxVec3 forceVec = dotP * -sideWaysDirection * friction;
        PxRigidBodyExt::addLocalForceAtLocalPos(*rigidDynamic, forceVec * deltaTime, PxVec3(0.f), PxForceMode::eFORCE);
    }
}

void engine::physics::components::CRaycastVehicle::evaluateInput(float deltaTime)
{
    if (bIsInAir)
        return;

    PxVec3 direction, forceVec;

    if (bUseControllerInput)
    {
        if (controllerInput.accelerationValue > 0)
        {
            direction = localTransform.rotate(PxVec3(0.f, 0.f, 1.f));
            direction.normalize();
            forceVec = direction * accelerationFactor;
            forceVec = projectVector(forceVec * controllerInput.accelerationValue, hit.block.normal);
            addForce(forceVec * deltaTime, PxForceMode::eFORCE);
        }

        if (controllerInput.breakValue > 0)
        {
            direction = localTransform.rotate(PxVec3(0.f, 0.f, -1.f));
            direction.normalize();
            forceVec = direction * accelerationFactor;
            forceVec = projectVector(forceVec * controllerInput.breakValue, hit.block.normal);
            addForce(forceVec * deltaTime, PxForceMode::eFORCE);
        }

        if (controllerInput.steeringValue != 0)
        {
            turningRate = matchForwardToSteer();
            addTorque(PxVec3(0.f, turningRate * controllerInput.steeringValue * deltaTime, 0.f), PxForceMode::eFORCE);
        }
    } else
    {
        if (keyInput.isKeyWPressedDown)
        {
            direction = localTransform.rotate(PxVec3(0.f, 0.f, 1.f));
            direction.normalize();
            forceVec = direction * accelerationFactor;
            forceVec = projectVector(forceVec, hit.block.normal);
            addForce(forceVec * deltaTime, PxForceMode::eFORCE);
        }

        if (keyInput.isKeySPressedDown)
        {
            direction = localTransform.rotate(PxVec3(0.f, 0.f, -1.f));
            direction.normalize();
            forceVec = direction * accelerationFactor;
            forceVec = projectVector(forceVec, hit.block.normal);
            addForce(forceVec * deltaTime, PxForceMode::eFORCE);
        }

        if (keyInput.isKeyAPressedDown)
        {
            turningRate = matchForwardToSteer();
            addTorque(PxVec3(0.f, turningRate * deltaTime, 0.f), PxForceMode::eFORCE);
        }

        if (keyInput.isKeyDPressedDown)
        {
            turningRate = matchForwardToSteer();
            addTorque(PxVec3(0.f, -turningRate * deltaTime, 0.f), PxForceMode::eFORCE);
        }
    }
}

void engine::physics::components::CRaycastVehicle::setResetTransform(const PxTransform& newResetTransform)
{
    resetTransform = newResetTransform;
}

bool engine::physics::components::CRaycastVehicle::isInAir() const
{
    return bIsInAir;
}

const glm::vec3& engine::physics::components::CRaycastVehicle::getBoxHalfExtends() const
{
    return boxHalfExtends;
}


void engine::physics::components::CRaycastVehicle::setUseControllerInput(bool bShouldUseControllerInput)
{
    bUseControllerInput = bShouldUseControllerInput;
}

float engine::physics::components::CRaycastVehicle::getForwardSpeed() const
{
    const physx::PxTransform chassisTransform = rigidDynamic->getGlobalPose().transform(
            rigidDynamic->getCMassLocalPose());
    float velocity = rigidDynamic->getLinearVelocity().dot(chassisTransform.q.rotate(PxVec3(0.f, 0.f, 1.f)));
    return velocity > 0 ? velocity : 0;
}

float engine::physics::components::CRaycastVehicle::matchForwardToSteer()
{
    float currSpeed = getForwardSpeed();
    for (int i = 0; i < FORWARD_STEER_PAIRS * 2; i += 2)
    {
        if (currSpeed < forwardVsSteer[i])
            return forwardVsSteer[i + 1];
    }

    return forwardVsSteer[(FORWARD_STEER_PAIRS * 2) - 1];
}

void engine::physics::components::CRaycastVehicle::boost(float accelerationFactor)
{
    PxVec3 direction, forceVec;

    direction = localTransform.rotate(PxVec3(0.f, 0.f, 1.f));
    direction.normalize();
    forceVec = direction * accelerationFactor;
    forceVec = projectVector(forceVec, hit.block.normal);
    addForce(forceVec, PxForceMode::eFORCE);
}

void engine::physics::components::CRaycastVehicle::printLoc()
{
    glm::mat4 currT = getTransform();
    std::cout << currT[3][0] << ", " << currT[3][1] << ", " << currT[3][2] << std::endl;
}