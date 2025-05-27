//
// Created by CemOe on 24.08.2021.
//

#include <engine/physics/Physics.h>
#include <engine/renderer/Renderer.h>

#define USE_ARTICULATIONS false
#define USE_HEIGHTFIELDS false

engine::physics::PhysicsSystem::PhysicsSystem(engine::Engine* engine, physx::PxSceneDesc sceneDesc)
    : System(),
      defaultStaticFriction(0.5f),
      defaultDynamicFriction(0.5f),
      defaultRestitution(0.2f),
      defaultDensity(10.f),
      defaultAngularDamping(0.5f),
      defaultMaterial(nullptr),
      chassisMaterialDrivable(nullptr),
      chassisMaterialNonDrivable(nullptr)
{
    pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pxAllocator, pxErrorCallback);

    if(!pxFoundation)
        throw std::runtime_error("PxCreateFoundation failed!");

    pvd = PxCreatePvd(*pxFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST.c_str(), PVD_PORT,
                                                                  PVD_TIMEOUT);
    pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    pxPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *pxFoundation,
                                    physx::PxTolerancesScale(), true, pvd);

    if(USE_ARTICULATIONS)
        PxRegisterArticulations(*pxPhysics);

    if(USE_HEIGHTFIELDS)
        PxRegisterHeightFields(*pxPhysics);

    if(!pxPhysics)
        throw std::runtime_error("PxCreatePhysics failed!");

    pxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *pxFoundation,
                                physx::PxCookingParams(physx::PxTolerancesScale()));
    if (!pxCooking)
        throw std::runtime_error("PxCreateCooking failed!");

    physx::PxTolerancesScale scale;
    physx::PxCookingParams params(scale);
    // disable mesh cleaning - perform mesh validation on development configurations
    params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    // disable edge precompute, edges are set for each triangle, slows contact generation
    params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

    pxCooking->setParams(params);

    PxInitVehicleSDK(*pxPhysics);
    PxVehicleSetBasisVectors(physx::PxVec3(0.f, 1.f, 0.f), physx::PxVec3(0.f, 0.f, 1.f));
    PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);

    pxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = pxDispatcher;

    if(!sceneDesc.filterShader)
        sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

    if(!sceneDesc.simulationEventCallback)
    {
        collisionCallback = engine::physics::collision::CollisionCallback();
        sceneDesc.simulationEventCallback = &collisionCallback;
    }

    sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
    sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP;

    pxScene = pxPhysics->createScene(sceneDesc);
    pxScene->setFlag(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
    pxScene->setFlag(physx::PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS, true);

    physx::PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
    if(pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

void engine::physics::PhysicsSystem::registerSystem(engine::Engine* engine)
{
    handle_startup = engine->event_startup.subscribe([this](engine::Engine* e) {
        startup(e);
    });

    handle_tick = engine->event_tick.subscribe([this](float deltaTime) {
        tick(deltaTime);
    });

    handle_shutdown = engine->event_shutdown.subscribe([this](engine::Engine* e) {
        shutdown(e);
    });
}

void engine::physics::PhysicsSystem::startup(engine::Engine* engine)
{
    vehicleManager = engine::physics::vehicle::VehicleManager(this);

    accumulator = 0;

    defaultMaterial = pxPhysics->createMaterial(defaultStaticFriction,
                                               defaultDynamicFriction, defaultRestitution);

    chassisMaterialDrivable = pxPhysics->createMaterial(0.f, 0.f, 0.f);
    chassisMaterialNonDrivable = pxPhysics->createMaterial(1.f, 1.f, 0.f);

    vehicleContainer = engine->createContainer<physics::components::CRaycastVehicle>();
    vehicleContainer->create_component.subscribe([this](engine::physics::components::raycastVehicle_handle_t vehicleHandle){
        vehicleHandle->pxScene = pxScene;
        vehicleHandle->pxPhysics = pxPhysics;
        vehicleHandle->pxDefaultMat = defaultMaterial;

        event_float_t::handle_t handle_physicsStepEvent;
        event_void_t::handle_t handle_physicsEvent;

        handle_physicsStepEvent = event_stepPhysics.subscribe([vehicleHandle](float stepSize){
            vehicleHandle->stepPhysics(stepSize);
        });

        handle_physicsEvent = event_physicsShutdown.subscribe([vehicleHandle](){
            vehicleHandle->destroyPhysics();
        });

        raycastVehicleHandles_stepEvent.insert({vehicleHandle, handle_physicsStepEvent});
        raycastVehicleHandles_physicsEvent.insert({vehicleHandle, handle_physicsEvent});
    });

    vehicleContainer->delete_component.subscribe([this](engine::physics::components::raycastVehicle_handle_t vehicleHandle){
        event_stepPhysics.unsubscribe(raycastVehicleHandles_stepEvent[vehicleHandle]);
        event_physicsShutdown.unsubscribe(raycastVehicleHandles_physicsEvent[vehicleHandle]);
        raycastVehicleHandles_stepEvent.erase(vehicleHandle);
        raycastVehicleHandles_physicsEvent.erase(vehicleHandle);
    });

    vehicleWheeledContainer = engine->createContainer<physics::components::CVehicle4Wheeled>();
    vehicleWheeledContainer->create_component.subscribe([this](engine::physics::components::raycastWheeledVehicle_handle_t vehicleHandle){
        vehicleHandle->physicsSystem = this;
        vehicleHandle->pxScene = pxScene;
        vehicleHandle->pxPhysics = pxPhysics;
        vehicleHandle->pxDefaultMat = defaultMaterial;

        event_void_t::handle_t handle_physicsEvent;

        vehicleHandle->event_physicsInitialized.subscribe([this](physx::PxVehicleDrive** vehicle){
            vehicleManager.registerVehicle(vehicle);
        });

        handle_physicsEvent = event_physicsShutdown.subscribe([vehicleHandle](){
            vehicleHandle->destroyPhysics();
        });

        raycastWheeledHandles_physicsEvent.insert({vehicleHandle, handle_physicsEvent});
    });

    vehicleWheeledContainer->delete_component.subscribe([this](engine::physics::components::raycastWheeledVehicle_handle_t vehicleHandle){
        event_stepPhysics.unsubscribe(raycastWheeledHandles_stepEvent[vehicleHandle]);
        event_physicsShutdown.unsubscribe(raycastWheeledHandles_physicsEvent[vehicleHandle]);
        raycastWheeledHandles_stepEvent.erase(vehicleHandle);
        raycastWheeledHandles_physicsEvent.erase(vehicleHandle);
    });


    tankContainer = engine->createContainer<physics::components::CVehicleTank>();
    tankContainer->create_component.subscribe([this](engine::physics::components::vehicleTank_handle_t vehicleHandle){
        vehicleHandle->physicsSystem = this;
        vehicleHandle->pxScene = pxScene;
        vehicleHandle->pxPhysics = pxPhysics;
        vehicleHandle->pxDefaultMat = defaultMaterial;

        event_void_t::handle_t handle_physicsEvent;

        vehicleHandle->event_physicsInitialized.subscribe([this](physx::PxVehicleDrive** vehicle){
            vehicleManager.registerVehicle(vehicle);
        });

        handle_physicsEvent = event_physicsShutdown.subscribe([vehicleHandle](){
            vehicleHandle->destroyPhysics();
        });

        tankHandles_physicsEvent.insert({vehicleHandle, handle_physicsEvent});
    });

    tankContainer->delete_component.subscribe([this](engine::physics::components::vehicleTank_handle_t vehicleHandle){
        event_stepPhysics.unsubscribe(tankHandles_stepEvent[vehicleHandle]);
        event_physicsShutdown.unsubscribe(tankHandles_physicsEvent[vehicleHandle]);
        tankHandles_stepEvent.erase(vehicleHandle);
        tankHandles_physicsEvent.erase(vehicleHandle);
    });

    boxCollisionContainer = engine->createContainer<physics::components::CBoxCollision>();
    boxCollisionContainer->create_component.subscribe([this](engine::physics::components::boxCollision_handle_t boxHandle){
        boxHandle->pxScene = pxScene;
        boxHandle->pxPhysics = pxPhysics;
        boxHandle->pxDefaultMat = defaultMaterial;

        event_float_t::handle_t handle_physicsStepEvent;
        event_void_t::handle_t handle_physicsEvent;

        handle_physicsStepEvent = event_stepPhysics.subscribe([boxHandle](float stepSize){
            boxHandle->stepPhysics(stepSize);
        });

        handle_physicsEvent = event_physicsShutdown.subscribe([boxHandle](){
            boxHandle->destroyPhysics();
        });

        boxCollisionHandles_stepEvent.insert({boxHandle, handle_physicsStepEvent});
        boxCollisionHandles_physicsEvent.insert({boxHandle, handle_physicsEvent});
    });

    boxCollisionContainer->delete_component.subscribe([this](engine::physics::components::boxCollision_handle_t boxHandle){
        event_stepPhysics.unsubscribe(boxCollisionHandles_stepEvent[boxHandle]);
        boxCollisionHandles_stepEvent.erase(boxHandle);
        event_physicsShutdown.unsubscribe(boxCollisionHandles_physicsEvent[boxHandle]);
        boxCollisionHandles_physicsEvent.erase(boxHandle);
    });


    sphereCollisionContainer = engine->createContainer<physics::components::CSphereCollision>();
    sphereCollisionContainer->create_component.subscribe([this](engine::physics::components::sphereCollision_handle_t sphereHandle){
        sphereHandle->pxScene = pxScene;
        sphereHandle->pxPhysics = pxPhysics;
        sphereHandle->pxDefaultMat = defaultMaterial;

        event_float_t::handle_t handle_physicsStepEvent;
        event_void_t::handle_t handle_physicsEvent;

        handle_physicsStepEvent = event_stepPhysics.subscribe([sphereHandle](float stepSize){
            sphereHandle->stepPhysics(stepSize);
        });

        handle_physicsEvent = event_physicsShutdown.subscribe([sphereHandle](){
            sphereHandle->destroyPhysics();
        });

        sphereCollisionHandles_stepEvent.insert({sphereHandle, handle_physicsStepEvent});
        sphereCollisionHandles_physicsEvent.insert({sphereHandle, handle_physicsEvent});
    });

    sphereCollisionContainer->delete_component.subscribe([this](engine::physics::components::sphereCollision_handle_t sphereHandle){
        event_stepPhysics.unsubscribe(sphereCollisionHandles_stepEvent[sphereHandle]);
        sphereCollisionHandles_stepEvent.erase(sphereHandle);
        event_physicsShutdown.unsubscribe(sphereCollisionHandles_physicsEvent[sphereHandle]);
        sphereCollisionHandles_physicsEvent.erase(sphereHandle);
    });


    complexCollisionContainer = engine->createContainer<physics::components::CComplexCollision>();
    complexCollisionContainer->create_component.subscribe([this](engine::physics::components::complexCollision_handle_t complexCollisionHandle){
        complexCollisionHandle->pxScene = pxScene;
        complexCollisionHandle->pxPhysics = pxPhysics;
        complexCollisionHandle->pxDefaultMat = defaultMaterial;
        complexCollisionHandle->pxCooking = pxCooking;

        event_float_t::handle_t handle_physicsStepEvent;
        event_void_t::handle_t handle_physicsEvent;

        handle_physicsStepEvent = event_stepPhysics.subscribe([complexCollisionHandle](float stepSize){
            complexCollisionHandle->stepPhysics(stepSize);
        });

        handle_physicsEvent = event_physicsShutdown.subscribe([complexCollisionHandle](){
            complexCollisionHandle->destroyPhysics();
        });

        complexCollisionHandles_stepEvent.insert({complexCollisionHandle, handle_physicsStepEvent});
        complexCollisionHandles_physicsEvent.insert({complexCollisionHandle, handle_physicsEvent});
    });

    complexCollisionContainer->delete_component.subscribe([this](engine::physics::components::complexCollision_handle_t complexCollisionHandle){
        event_stepPhysics.unsubscribe(complexCollisionHandles_stepEvent[complexCollisionHandle]);
        complexCollisionHandles_stepEvent.erase(complexCollisionHandle);
        event_physicsShutdown.unsubscribe(complexCollisionHandles_physicsEvent[complexCollisionHandle]);
        complexCollisionHandles_physicsEvent.erase(complexCollisionHandle);
    });


    heightFieldContainer = engine->createContainer<physics::components::CHeightField>();
    heightFieldContainer->create_component.subscribe([this](engine::physics::components::heightField_handle_t heightFieldHandle){
        heightFieldHandle->pxScene = pxScene;
        heightFieldHandle->pxPhysics = pxPhysics;
        heightFieldHandle->pxDefaultMat = defaultMaterial;
        heightFieldHandle->pxCooking = pxCooking;

        event_float_t::handle_t handle_physicsStepEvent;
        event_void_t::handle_t handle_physicsEvent;

        handle_physicsEvent = event_physicsShutdown.subscribe([heightFieldHandle](){
            heightFieldHandle->destroyPhysics();
        });

        heightFieldHandles_physicsEvent.insert({heightFieldHandle, handle_physicsEvent});
    });

    heightFieldContainer->delete_component.subscribe([this](engine::physics::components::heightField_handle_t heightFieldHandle){
        event_physicsShutdown.unsubscribe(heightFieldHandles_physicsEvent[heightFieldHandle]);
        heightFieldHandles_physicsEvent.erase(heightFieldHandle);
    });
}

void engine::physics::PhysicsSystem::tick(const float deltaTime)
{
    accumulator += deltaTime;

    if(accumulator < STEP_SIZE)
      return;

    accumulator -= STEP_SIZE;

    // Step vehicle physics first
    stepVehiclePhysics();

    event_stepPhysics.fire(STEP_SIZE);

    pxScene->simulate(STEP_SIZE);
    pxScene->fetchResults(true);

    // retrieve array of actors that moved
    physx::PxU32 nbActiveActors;
    physx::PxActor** activeActors = pxScene->getActiveActors(nbActiveActors);

    // update each render object with the new transform
    for (physx::PxU32 i = 0; i < nbActiveActors; i++)
    {
        if (activeActors[i]->userData)
        {
            auto* physicsComponent = static_cast<physics::components::CPhysicsComponent*>(activeActors[i]->userData);
            physicsComponent->updateTransform();
        }
    }

    event_afterPhysicsTick.fire();
}

void engine::physics::PhysicsSystem::shutdown(engine::Engine* engine)
{
    engine->event_startup.unsubscribe(handle_startup);
    engine->event_tick.unsubscribe(handle_tick);
    engine->event_shutdown.unsubscribe(handle_shutdown);

    event_physicsShutdown.fire();

    physx::PxCloseVehicleSDK();
    pxScene->release();
    pxCooking->release();
    pxDispatcher->release();
    pxPhysics->release();
    pxFoundation->release();
}

physx::PxMaterial* engine::physics::PhysicsSystem::getDefaultMaterial() const
{
    return defaultMaterial;
}

physx::PxConvexMesh* engine::physics::PhysicsSystem::createConvexMesh(physx::PxVec3* vertices, int numVertices) const
{
    // Create descriptor for convex mesh
    physx::PxConvexMeshDesc convexDesc;
    convexDesc.points.count			= numVertices;
    convexDesc.points.stride		= sizeof(physx::PxVec3);
    convexDesc.points.data			= vertices;
    convexDesc.flags				= physx::PxConvexFlag::eCOMPUTE_CONVEX;

    physx::PxConvexMesh* convexMesh = NULL;
    physx::PxDefaultMemoryOutputStream buf;
    if(pxCooking->cookConvexMesh(convexDesc, buf))
    {
        physx::PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
        convexMesh = pxPhysics->createConvexMesh(id);
    }

    return convexMesh;
}

void engine::physics::PhysicsSystem::stepVehiclePhysics()
{
    vehicleManager.stepVehiclePhysics(STEP_SIZE);
}

physx::PxDefaultAllocator* engine::physics::PhysicsSystem::getPxAllocator()
{
    return &pxAllocator;
}

physx::PxPhysics* engine::physics::PhysicsSystem::getPxPhysics() const
{
    return pxPhysics;
}

physx::PxScene* engine::physics::PhysicsSystem::getPxScene() const
{
    return pxScene;
}

physx::PxMaterial* engine::physics::PhysicsSystem::getChassisMaterialDrivable() const
{
    return chassisMaterialDrivable;
}

physx::PxMaterial* engine::physics::PhysicsSystem::getChassisMaterialNonDrivable() const
{
    return chassisMaterialNonDrivable;
}
