# Quick start guide

## Setup
1. Open the tools folder and unzip Toolchain-CLION
![](images/quick-start-1.PNG)
2. After unzipping open the code folder with CLion
![](images/quick-start-2.png)
3. The "Open Project Wizard" should open and display the different CMake profiles. There click on "Manage toolchains...". If the window is not display at the start you can get to the toolchain settings by opening File > Setting > Build, Execution, Deployment > Toolchains
![](images/quick-start-3.png)
4. In the toolchain window add a new toolchain by pressing on "+" and selecting Visual Studio
5. Set the architecture to amd64.
6. Then set up the path for the toolset and CMake we unzipped at (1)
![](images/quick-start-4.png)
7. Then return to the CMake profiles or open it through File > Setting > Build, Execution, Deployment > CMake
8. There click on + and ot create the Release configuration if you want to
9. Then in both profiles set the toolchain to the one we just created. As for the generator you can use the default one called *Ninja* or *NMake Makesfiles*. I suggest using *Ninja* since it seems to be faster
10. After applying the changes and closing the window the project CMakeList should run. If you encounter an error like this just click on retry and it shpould work the second time around and you are done with the setup.
![](images/quick-start-5.png)

## Starting up the game prototype
1. Select the game target in the upper right corner where the "play button" is located
![](images/quick-start-6.png)
2. Press the "play-button"
3. Finally have fun! :)


## Creating your own game

### Setting up the game
1. Create a new folder in the code directory in CLion and name it however you want to
2. Create your CMakeList our just copy this one and replace [Project-name] with your project name
```
cmake_minimum_required(VERSION 3.18)

# Glob "okay" for CMAKE 3.12+ using CONFIGURE_DEPENDS flag!
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "src/**.cpp")
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "src/**.h")

# set executable name
set(EXE_FILE [Project-Name])

# add the executable target
add_executable(${EXE_FILE} ${SOURCE_LIST} ${HEADER_LIST} ${IMGUI_SOURCE_LIST})

# require C++ 17 compiler
target_compile_features(${EXE_FILE} PRIVATE cxx_std_17)

# link with dependencies
target_link_libraries(${EXE_FILE} PRIVATE glad glfw glm Engine Renderer Scene Physics UserInterface Audio)

target_compile_definitions(${EXE_FILE} PRIVATE DEBUG_ASSET_ROOT=${PROJECT_SOURCE_DIR})

# IDE should put the headers in a nice place
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source" FILES ${SOURCE_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Private Header" FILES ${HEADER_LIST})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/generated" PREFIX "Generated Files" FILES ${GENERATED_FILES_LIST})
```
3. After creating your CMakeList go to the root CMakeList and add your project with this command :
	```add_subdirectory([directory-name])```\
	Now you should be able to select your game as a target.
4. Create your "src" directory inside of your project folder.
5. Inside the folder create your first source files: Game.cpp and Game.h and you main.cpp

The Game.h annd Game.cpp should look something like this
```
#pragma once
#include <engine/Engine.h>
#include <engine/renderer/Renderer.h>
#include <engine/scene/Scene.h>
#include <engine/physics/Physics.h>
#include <engine/audio/Audio.h>

namespace demo
{
    class Game
    {
    public:
        Game();

        void run();

        engine::renderer::RenderSystem* rendererSystem;

        engine::scene::SceneSystem* sceneSystem;

        engine::physics::PhysicsSystem* physicsSystem;

        engine::ui::UserInterface* userInterfaceSystem;

        engine::audio::Audio* audioSystem;

        engine::world::World world = engine::world::World(&engine);

    private:
        engine::Engine engine = engine::Engine();
        
    private:
        void startup();

        void afterSystemStartup();

        void tick(float deltaTime);

        void beforeSystemShutdown();
    };
}
```

```

#include "Game.h"

demo::Game::Game() : engine(), world(&engine)
{
    sceneSystem = engine.addSystem<engine::scene::SceneSystem, engine::Engine*>(&engine);
    rendererSystem = engine.addSystem<engine::renderer::RenderSystem>();
    userInterfaceSystem = engine.addSystem<engine::ui::UserInterface>();
    audioSystem = engine.addSystem<engine::audio::Audio>();
    auto sceneDesc = physx::PxSceneDesc(physx::PxTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
    sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP;
    physicsSystem = engine.addSystem<engine::physics::PhysicsSystem, 
    	engine::Engine*>(&engine, sceneDesc);
    	
    engine.event_startup.subscribe([this](engine::Engine* engine) {
        startup();
    });

    engine.event_afterStartup.subscribe([this](engine::Engine* engine) {
        afterSystemStartup();
    });

    engine.event_tick.subscribe([this](float deltaTime) {
        tick(deltaTime);
    });
}

void demo::Game::startup()
{

}

void demo::Game::run()
{
    engine.run();
}

void demo::Game::afterSystemStartup()
{
  /* Create your actors here */
}

void demo::Game::tick(float deltaTime)
{

}

```

With these files we have created the subsystems we want, registered the subsystems to the engine and subscribed to the engine events to fire our startup(), afterSystemStartup() and tick() function.

In the main you then just need to create an instance of that game class we just created and call the run() function.

```
#pragma once
#include "Game.h"

int main()
{
    demo::Game game = demo::Game();
    game.run();
};
```

This following code snippet can be added above the main() function if you have a iGPU and want the process to use your dedicated graphics card

```
/* For NVIDIA cards */
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

/* For AMD cards */
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

```

### Creating a demo scene

#### Placing actors into the scene

Let us start with placing a global directional light and a camera to the scene. For that we can use the pre-exisiting actors in the module renderer. Go into your Game.h and include the following header:


```
#include <engine/renderer/world/ALight.h>
#include <engine/renderer/world/ACamera.h>
```

then declare these two shared pointer that we are gonna use to store the references to the actors.

```
std::shared_ptr<engine::renderer::world::ALight> light;
std::shared_ptr<engine::renderer::world::ACamera> camera;
```

Now open the Game.cpp and go to your afterSystemStarup() function and create your actors by calling the temlate function createActor() of the world class.
This would look like this:

```
void demo::Game::afterSystemStartup()
{
    glm::mat4 transform = glm::mat4(1.f);

    // Light
    transform = glm::mat4(1.f);
    light = world.createActor<engine::renderer::world::ALight>(transform, &world);

    // Camera
    transform = glm::translate(transform, glm::vec3(0.f, 20.f, 50.f));
    /* 
     * The third parameter is a lamba that returns the point the camera 
     * is supposed to look at 
    */
    camera = world.createActor<engine::renderer::world::ACamera>(transform, &world,
                                             []() { return glm::vec3(0.f, 0.f, 0.f); });
```

If you compile the target now you should see a white screen. 
Lets fill the scene with some actors that we can actually see.

#### Creating actors and adding components

To create a actor you to need to inherit from the engine::world::AActor class.
To add components to your actor you need to delcare the respective **handle** not the component itself. This would look something like this:

```
#include <engine/world/AActor.h>
#include <engine/scene/components/CTransform.h>
#include <engine/renderer/components/CModel.h>
#include <engine/physics/components/collision/CBoxCollision.h>


namespace demo::world
{
    class ACube : public engine::world::AActor
    {
    public:
        ACube(glm::mat4 transform, engine::world::World* world);

    private:
    	// Component that is part of the scene graph and tranform
        engine::scene::components::transform_handle_t transformComp;

        // The mesh we want to respresent our actor
        engine::renderer::components::model_handle_t meshComp;

        // A simple box collision, since were a going to load a model of a cube
        engine::physics::components::boxCollision_handle_t collisionComp;
    };
}
```

To create the components now and them to our actor we need to call the templated function of our entity class. After creating the component we need to initialize it with the respective ConstructionInfo struct located in the component. Your class should look somthing like this:


```
#include "ACube.h"

demo::world::ACube::ACube(glm::mat4 transform, engine::world::World* world) : AActor(world)
{
    // Init transform component
    transformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "cube.obj";
    meshComp->init(&meshInfo);

    // Init collision
    collisionComp = rootEntity.addComponent<engine::physics::components::CBoxCollision>();
    engine::physics::components::CBoxCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.boxHalfExtent = glm::vec3(1.f, 1.f, 1.f);
    collisionInfo.bIsDynamic = true;
    collisionInfo.bIsKinematic = false;
    collisionInfo.bIsSimulationShape = true;
    collisionInfo.mass = 10.f;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bActiveFromStart = true;
    collisionComp->init(&collisionInfo);
}
```

Now we could add a simple cube to the scene, you would not see anything. That is due to fact that the components are being not notified about the scene traveser traversing the transform component.

In order to update our transform so that we can see the mesh and see movement as well, we need to subscribe to the *event_visit* event of our transformComp.

Just add the following lines to your ACube.cpp at the end of the constructor. This notifies our other components that the scene traverser visited our transform component, and the scene updates should take place now. So the mesh component draws its mesh and the collision components updates our transform component.

```
    transformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->initialVisit(traverser);
        collisionComp->initialVisit(traverser);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->visit(traverser);
        collisionComp->visit(traverser);
    });
```

The constructor should now look like this:

```
demo::world::ACube::ACube(glm::mat4 transform, engine::world::World* world) : AActor(world)
{
    // Init transform component
    transformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "cube.obj";
    meshComp->init(&meshInfo);

    collisionComp = rootEntity.addComponent<engine::physics::components::CBoxCollision>();
    engine::physics::components::CBoxCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.boxHalfExtent = glm::vec3(1.f, 1.f, 1.f);
    collisionInfo.bIsDynamic = true;
    collisionInfo.bIsKinematic = false;
    collisionInfo.bIsSimulationShape = true;
    collisionInfo.mass = 10.f;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bActiveFromStart = true;
    collisionComp->init(&collisionInfo);

    transformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->initialVisit(traverser);
        collisionComp->initialVisit(traverser);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->visit(traverser);
        collisionComp->visit(traverser);
    });
}
```

Now you should see a falling cube if you add the cube to scene.

#### Before adding vehicles to the scene

Before proceeding to the vehicles we should add a platform to drive on.

```
#pragma once
#include <engine/world/AActor.h>
#include <engine/scene/components/CTransform.h>
#include <engine/renderer/components/CModel.h>
#include <engine/physics/components/collision/CComplexCollision.h>

namespace demo::world
{
    class APlatform : public engine::world::AActor
    {
    public:
        APlatform(glm::mat4 transform, engine::world::World* world);

    private:
        engine::scene::components::transform_handle_t transformComp;

        engine::scene::components::transform_handle_t cubeScaling;

        engine::renderer::components::model_handle_t meshComp;

        engine::physics::components::complexCollision_handle_t collisionComp;

        std::vector<std::vector<glm::vec3>> vertices;

        std::vector<std::vector<unsigned int>> indices;
    };
}
```

```
#include "APlatform.h"
#include "engine/physics/vehicle/VehicleUtil.h"

demo::world::APlatform::APlatform(glm::mat4 transform, engine::world::World* world) : AActor(world)
{
    // Init transform component
    transformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    // Add another transform component to scale the cube.
    cubeScaling = rootEntity.addComponent<engine::scene::components::CTransform>();
    transformInfo.transform = glm::scale(glm::mat4(1.f), glm::vec3(100.f, 10.f, 100.f));
    cubeScaling->init(&transformInfo);
    // Do not forget to add the transform component as a child. 
    // This is only possible with transform components
    transformComp->addChild(cubeScaling->getId());

    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "Cube.obj";
    meshComp->init(&meshInfo);

    // Init mesh collision
    collisionComp = rootEntity.addComponent<engine::physics::components::CComplexCollision>();
    engine::physics::components::CComplexCollision::ConstructionInfo collisionInfo;
    collisionInfo.instantiator = this;
    collisionInfo.bIsDynamic = false;
    collisionInfo.bIsSimulationShape = true;
    collisionInfo.bShouldDeactivateAfterCollision = false;
    collisionInfo.timeActive = -1.f;
    collisionInfo.bUpdateLocation = false;
    meshComp->getAllVertices(vertices);
    meshComp->getAllIndices(indices);
    collisionInfo.vertices = &vertices;
    collisionInfo.indices = &indices;
    collisionInfo.bActiveFromStart = true;

    physx::PxFilterData qryFilterData;
    qryFilterData.word3 = static_cast<physx::PxU32>(engine::physics::vehicle::DRIVABLE_SURFACE);
    collisionInfo.qryFilterData = qryFilterData;

    physx::PxFilterData simulationFilterData;
    simulationFilterData.word0 = engine::physics::vehicle::COLLISION_FLAG_GROUND;
    simulationFilterData.word1 = engine::physics::vehicle::COLLISION_FLAG_GROUND_AGAINST;
    collisionInfo.simulationFilterData = simulationFilterData;

    collisionComp->init(&collisionInfo);

    cubeScaling->event_init.subscribe([this](engine::scene::TreeTraverser* traverser){
        meshComp->initialVisit(traverser);
        collisionComp->initialVisit(traverser);
    });

    cubeScaling->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser){
        meshComp->visit(traverser);
        collisionComp->visit(traverser);
    });
}
```

Important is the **qryFilterData** and the **simulationFilterData**. With out them our suspension raycast will not hit anything.


#### Adding vehicles

The engine provides each vehicle type with a basic actor.

- engine::physics::world::ARaycastVehicle
- engine::physics::world::ARaycastVehicle4Wheeled
- engine::physics::world::ATank

So we just need to inherit from these basic implementations and add a mesh component to it so we can actually see the vehicle moving.

In the code folder you should see the project I just described called *demo* already finished with all three vehicles placed into the scene. With the keys 1, 2 and 3 you can switch between the vehicles you want to control.

If you want to try out things like triggers, collision events, contact manipualtion, particle system, access the debug window and the other advanced features we could not cover here, just take a look at the game prototype where every feature of the engine is shown in practice.



