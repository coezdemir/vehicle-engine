//
// Created by Cem on 24.05.2022.
//

#include "Car4W.h"

game::actors::Car4W::Car4W(glm::mat4 transform, engine::world::World* world) : ARaycastVehicle4Wheeled(transform, world)
{
    glm::mat4 camTransform = glm::mat4(1.f);
    camTransform = glm::translate(camTransform, glm::vec3(0.f, 5.f, -18.f));
    camTransformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = camTransform;
    camTransformComp->init(&transformInfo);

    cameraComp = getRootEntity().addComponent<engine::renderer::components::CCamera>();
    engine::renderer::components::CCamera::ConstructionInfo constructionInfo =
            engine::renderer::components::CCamera::ConstructionInfo();
    constructionInfo.instantiator = this;
    constructionInfo.targetLoc = [this]() { return transformComp->getTransform()[3]; };
    constructionInfo.cameraType = engine::renderer::components::CT_PERSPECTIVE;
    cameraComp->init(&constructionInfo);

    handle_keyInput =
            world->event_keyInput.subscribe([this](const GLFWwindow* window, int key, int scancode, int action, int mods) {
                raycastVehicleComp->processInput(key, action);
            });

    handle_controllerInput =
            world->event_controllerInput.subscribe([this](int controllerID, GLFWgamepadstate controllerState) {
                raycastVehicleComp->processInput(controllerState);
            });
}
