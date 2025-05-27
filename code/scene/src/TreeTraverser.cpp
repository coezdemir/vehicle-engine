//
// Created by CemOe on 22.08.2021.
//
#include <engine/scene/TreeTraverser.h>
#include <engine/world/AActor.h>
#include <engine/renderer/SkyBox.h>

engine::scene::TreeTraverser::TreeTraverser(engine::Engine* engine)
        : engine(engine), numberOfCameras(0), skybox(nullptr)
{
    stack.push(glm::mat4(1.f));
    bIsCinematicCameraPresent = false;
    bRenderCinematic = false;
}

void engine::scene::TreeTraverser::emptyStack()
{
    while (!stack.empty())
        stack.pop();

    stack.push(glm::mat4(1.f));
}

glm::mat4 engine::scene::TreeTraverser::getView() const
{
    return view;
}

glm::mat4 engine::scene::TreeTraverser::getProjection() const
{
    return projection;
}

glm::mat4 engine::scene::TreeTraverser::getStackTop() const
{
    return stack.top();
}

void engine::scene::TreeTraverser::pushStack(glm::mat4 mat4)
{
    stack.push(stack.top() * mat4);
}

bool engine::scene::TreeTraverser::shouldRenderCinematic() const
{
    return bRenderCinematic;
}

glm::mat4 engine::scene::TreeTraverser::popStack()
{
    glm::mat4 ret = stack.top();
    stack.pop();
    return ret;
}

glm::vec3 engine::scene::TreeTraverser::getEye() const
{
    return eye;
}

void engine::scene::TreeTraverser::setEye(const glm::vec3& newEye)
{
    eye = newEye;
}

void engine::scene::TreeTraverser::setView(const glm::mat4& newView)
{
    view = newView;
}

void engine::scene::TreeTraverser::setProjection(const glm::mat4& newProjection)
{
    projection = newProjection;
}

void engine::scene::TreeTraverser::initialTraversal(SceneGraph* graph)
{
    for (const engine::components::id_t& transformId : graph->tree)
        initNext(engine->getContainer<scene::components::CTransform>()->get(transformId));
}

void engine::scene::TreeTraverser::initNext(scene::components::transform_handle_t transformComp)
{
    currentPath.push_back(transformComp);

    currTransformComp = transformComp;

    currTransformComp->initialVisit(this);

    std::vector<engine::components::id_t> children = currTransformComp->getChildren();

    for (const engine::components::id_t& id : children)
        initNext(engine->getContainer<scene::components::CTransform>()->get(id));

    stack.pop();

    currentPath.pop_back();
}

int engine::scene::TreeTraverser::getNumberOfViewports() const
{
    return numberOfCameras;
}

void engine::scene::TreeTraverser::traverseScene(SceneGraph* graph)
{
    if(!bRenderCinematic)
    {
        for (int i = 0; i < numberOfCameras; i++)
        {
            cameraTraversal(i);

            traverseTree(graph);

            renderSkybox();
        }
    }
    else
    {
        cameraTraversal(cinematicCameraID);

        traverseTree(graph);

        renderSkybox();
    }
}

void engine::scene::TreeTraverser::visitNext(scene::components::transform_handle_t transformComp)
{
    currTransformComp = transformComp;

    currTransformComp->visit(this);

    std::vector<engine::components::id_t> children = currTransformComp->getChildren();

    for (const engine::components::id_t& id : children)
        visitNext(engine->getContainer<scene::components::CTransform>()->get(id));

    stack.pop();
}

const engine::scene::components::transform_handle_t& engine::scene::TreeTraverser::getCurrTransformComp() const
{
    return currTransformComp;
}

void engine::scene::TreeTraverser::createViewport(int cameraID)
{
    float windowWidth = engine->getContext().getWidth();
    float windowHeight = engine->getContext().getHeight();

    if(numberOfCameras == 1 || bRenderCinematic)
    {
        glViewport(0, 0, windowWidth, windowHeight);
        return;
    }

    if (numberOfCameras < 3)
    {
        switch (cameraID)
        {
            case 0:
                glViewport(0, 0, windowWidth / 2, windowHeight);
                break;
            case 1:
                glViewport(windowWidth / 2, 0, windowWidth / 2, windowHeight);
                break;
            default:
                return;
        }
    }
    else
    {
        switch (cameraID)
        {
            case 0:
                glViewport(0, windowHeight / 2, windowWidth / 2, windowHeight / 2);
                break;
            case 1:
                glViewport(windowWidth / 2, windowHeight / 2, windowWidth / 2, windowHeight / 2);
                break;
            case 2:
                glViewport(0, 0, windowWidth / 2, windowHeight / 2);
                break;
            case 3:
                glViewport(windowWidth / 2, 0, windowWidth / 2, windowHeight / 2);
                break;
            default:
                return;
        }
    }
}

int engine::scene::TreeTraverser::getCurrentCameraID() const
{
    return currentCameraID;
}

void engine::scene::TreeTraverser::setSkybox(engine::scene::ITraversableObject* newSkybox)
{
    skybox = newSkybox;
}

int engine::scene::TreeTraverser::addCamera(const engine::components::id_t& parent)
{
    if (numberOfCameras < 4 || (numberOfCameras < 5 && bIsCinematicCameraPresent))
    {
        numberOfCameras++;
        int cameraID = numberOfCameras - 1;
        cameraPaths.insert({cameraID, currentPath});
        return cameraID;
    }

    return -1;
}

bool engine::scene::TreeTraverser::isCinematicCameraPresent() const
{
    return bIsCinematicCameraPresent;
}

int engine::scene::TreeTraverser::addCinematicCamera(const engine::components::id_t& camera)
{
    if (!bIsCinematicCameraPresent)
    {
        bIsCinematicCameraPresent = true;
        numberOfCameras++;
        cinematicCameraID = numberOfCameras - 1;
        cameraPaths.insert({cinematicCameraID, currentPath});
        return cinematicCameraID;
    }

    return -1;
}

void engine::scene::TreeTraverser::renderSkybox()
{
    glDepthFunc(GL_LEQUAL);
    if(skybox != nullptr)
        skybox->visit(this);
    glDepthFunc(GL_LESS);
}

void engine::scene::TreeTraverser::traverseTree(const SceneGraph* graph)
{
    for (const engine::components::id_t transformId : graph->tree)
        visitNext(engine->getContainer<scene::components::CTransform>()->get(transformId));
}

void engine::scene::TreeTraverser::cameraTraversal(const int cameraID)
{
    currentCameraID = cameraID;

    createViewport(currentCameraID);

    // Traverse path to camera with the current cameraID and setup view, projection and model
    if(!cameraPaths[currentCameraID].empty())
    {
        for (int i = 0; i < cameraPaths[currentCameraID].size() - 1; ++i)
        {
            currTransformComp = cameraPaths[currentCameraID][i];
            currTransformComp->visitWithoutEvent(this);
        }

        currTransformComp = cameraPaths[currentCameraID][cameraPaths[currentCameraID].size() - 1];
        currTransformComp->visit(this);
        emptyStack();
    }
}

void engine::scene::TreeTraverser::setRenderCinematic(const bool bNewRenderCinematic)
{
    bRenderCinematic = bNewRenderCinematic;
}
