//
// Created by Cem on 26.07.2022.
//

#include "ADemoTank.h"

demo::world::ADemoTank::ADemoTank(glm::mat4 transform, engine::world::World* world)
    : ATank(transform, world), bControlsActive(false)
{
    glm::mat4 meshTransform = glm::mat4(1.f);
    meshTransform = glm::scale(meshTransform, glm::vec3(10.f, 10.f, 10.f));
    meshTransformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = meshTransform;
    meshTransformComp->init(&transformInfo);
    transformComp->addChild(meshTransformComp->getId());

    // Init mesh component
    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "demo/tank/tank.obj";
    meshComp->init(&meshInfo);

    transformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser) {
        tankComp->initialVisit(traverser);
    });

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        tankComp->visit(traverser);
    });

    meshTransformComp->event_init.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->initialVisit(traverser);
    });

    meshTransformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->visit(traverser);
    });

    world->event_keyInput.unsubscribe(handle_keyInput);
    world->event_controllerInput.unsubscribe(handle_controllerInput);
}

void demo::world::ADemoTank::activateControls()
{
    if (!bControlsActive)
    {
        handle_keyInput =
                world->event_keyInput.subscribe(
                        [this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
                            tankComp->processInput(key, action);
                        });

        handle_controllerInput =
                world->event_controllerInput.subscribe([this](int controllerID, GLFWgamepadstate controllerState) {
                    tankComp->processInput(controllerState);
                });

        bControlsActive = true;
    }
}

void demo::world::ADemoTank::deactivateControls()
{
    if (bControlsActive)
    {
        world->event_keyInput.unsubscribe(handle_keyInput);
        world->event_controllerInput.unsubscribe(handle_controllerInput);
        bControlsActive = false;
    }
}
