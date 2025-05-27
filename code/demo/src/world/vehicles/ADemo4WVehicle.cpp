//
// Created by Cem on 26.07.2022.
//

#include "ADemo4WVehicle.h"

demo::world::ADemo4WVehicle::ADemo4WVehicle(glm::mat4 transform, engine::world::World* world)
    : ARaycastVehicle4Wheeled(transform, world), bControlsActive(false)
{
    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "demo/car/car.obj";
    meshComp->init(&meshInfo);

    transformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser) {
        raycastVehicleComp->initialVisit(traverser);
        meshComp->initialVisit(traverser);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        raycastVehicleComp->visit(traverser);
        meshComp->visit(traverser);
    });

    world->event_keyInput.unsubscribe(handle_keyInput);
    world->event_controllerInput.unsubscribe(handle_controllerInput);
}

void demo::world::ADemo4WVehicle::activateControls()
{
    if (!bControlsActive)
    {
        handle_keyInput =
                world->event_keyInput.subscribe(
                        [this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
                            raycastVehicleComp->processInput(key, action);
                        });

        handle_controllerInput =
                world->event_controllerInput.subscribe([this](int controllerID, GLFWgamepadstate controllerState) {
                    raycastVehicleComp->processInput(controllerState);
                });

        bControlsActive = true;
    }
}

void demo::world::ADemo4WVehicle::deactivateControls()
{
    if (bControlsActive)
    {
        world->event_keyInput.unsubscribe(handle_keyInput);
        world->event_controllerInput.unsubscribe(handle_controllerInput);

        bControlsActive = false;
    }
}
