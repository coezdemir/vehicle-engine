//
// Created by CemOe on 19.09.2021.
//

#include <engine/renderer/components/CCamera.h>
#include <engine/scene/TreeTraverser.h>
#include <engine/renderer/Renderer.h>

engine::renderer::components::CCamera::CCamera(const engine::components::id_t id, const entities::id_t owner)
        : CComponent(id, owner), bIsCinematic(false), cameraID(-1)
{

}

void engine::renderer::components::CCamera::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* cameraConstruction =
            dynamic_cast<const engine::renderer::components::CCamera::ConstructionInfo*>(constructionInfo);
    assert(cameraConstruction && "Please use the correct ConstructionInfo: CCamera::ConstructionInfo");

    cameraType = cameraConstruction->cameraType;
    targetLoc = cameraConstruction->targetLoc;
    up = cameraConstruction->up;
    fovy = cameraConstruction->fovy;
    left = cameraConstruction->left;
    right = cameraConstruction->right;
    bottom = cameraConstruction->bottom;
    top = cameraConstruction->top;
    zNear = cameraConstruction->zNear;
    zFar = cameraConstruction->zFar;
    bIsCinematic = cameraConstruction->bIsCinematic;
}

void engine::renderer::components::CCamera::initialVisit(engine::scene::TreeTraverser* traverser)
{
    bInitialTraversed = true;

    if (bIsCinematic)
        cameraID = traverser->addCinematicCamera(traverser->getCurrTransformComp()->getId());
    else
        cameraID = traverser->addCamera(traverser->getCurrTransformComp()->getId());
}

void engine::renderer::components::CCamera::visit(scene::TreeTraverser* traverser)
{
    if (!bInitialTraversed)
    {
        initialVisit(traverser);
        return;
    }

    if (!bIsCinematic && traverser->shouldRenderCinematic() ||
        (bIsCinematic && !traverser->shouldRenderCinematic()))
        return;

    if (cameraID == traverser->getCurrentCameraID())
    {
        traverser->setEye(traverser->getStackTop()[3]);
        glm::mat4 lookAt = glm::lookAt(glm::vec3(traverser->getEye()), targetLoc(), glm::vec3(up));
        traverser->setView(lookAt);

        if (cameraType == CT_PERSPECTIVE)
            traverser->setProjection(
                    glm::perspective(glm::radians(fovy),
                                     calculateAspect(traverser->getNumberOfViewports()),
                                     zNear, zFar)
            );
        else
            traverser->setProjection(glm::ortho(left, top, bottom, top, zNear, zFar));

        shader->use();
        shader->setMatrix("view", traverser->getView());
        shader->setMatrix("projection", traverser->getProjection());
        shader->setMatrix("model", traverser->getStackTop());
        shader->setVector("viewPos", traverser->getEye());
    }
}

float engine::renderer::components::CCamera::calculateAspect(const int numberOfViewports)
{
    switch (numberOfViewports)
    {
        default:
        case 1:
            return windowWidth / windowHeight;
        case 2:
            return (windowWidth / 2) / windowHeight;
        case 3:
        case 4:
            return (windowWidth / 2) / (windowHeight / 2);
    }
}

glm::vec3 engine::renderer::components::CCamera::getTargetActorLoc() const
{
    return targetLoc();
}

const glm::vec4& engine::renderer::components::CCamera::getUp() const
{
    return up;
}

float engine::renderer::components::CCamera::getFovy() const
{
    return fovy;
}

float engine::renderer::components::CCamera::getZNear() const
{
    return zNear;
}

float engine::renderer::components::CCamera::getZFar() const
{
    return zFar;
}