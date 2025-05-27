//
// Created by Cem on 26.07.2022.
//

#include "ADemoRaycastVehicle.h"

demo::world::ADemoRaycastVehicle::ADemoRaycastVehicle(glm::mat4 transform, engine::world::World* world)
        : ARaycastVehicle(transform, world), bControlsActive(true)
{
    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "game/spacecraft/player1/space-vehicle.obj";
    meshComp->init(&meshInfo);

    transformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser){
        raycastVehicleComp->initialVisit(traverser);
        meshComp->initialVisit(traverser);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser){
        raycastVehicleComp->visit(traverser);
        meshComp->visit(traverser);
    });
}

void demo::world::ADemoRaycastVehicle::activateControls()
{
    if(!bControlsActive)
    {
        handle_keyInput =
                world->event_keyInput.subscribe(
                        [this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
                            raycastVehicleComp->processKeyInput(key, action);
                        });

        handle_controllerInput =
                world->event_controllerInput.subscribe([this](int controllerID, GLFWgamepadstate controllerState) {
                    raycastVehicleComp->processControllerInput(controllerState);
                });
        bControlsActive = true;
    }
}

void demo::world::ADemoRaycastVehicle::deactivateControls()
{
    if(bControlsActive)
    {
        world->event_keyInput.unsubscribe(handle_keyInput);
        world->event_controllerInput.unsubscribe(handle_controllerInput);
        bControlsActive = false;
    }
}
