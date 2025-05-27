//
// Created by CemOe on 22.08.2021.
//

#include <engine/scene/components/CTransform.h>
#include <engine/scene/TreeTraverser.h>
#include <glm/ext/matrix_transform.hpp>

using namespace engine::scene::components;

CTransform::CTransform(const engine::components::id_t id, const engine::entities::id_t owner)
    : CComponent(id, owner)
{

}

void CTransform::init(const engine::components::ConstructionInfo* const constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    const auto* transformConstruction =
            dynamic_cast<const CTransform::ConstructionInfo*>(constructionInfo);
    assert(transformConstruction && "Please use the correct ConstructionInfo: CTransform::ConstructionInfo");

    setLocalTransform(transformConstruction->transform);
}

void CTransform::initialVisit(engine::scene::TreeTraverser* traverser)
{
    traverser->pushStack(localTransform);
    setTransform(traverser->getStackTop());
    event_init.fire(traverser);
}

void CTransform::visit(engine::scene::TreeTraverser* const traverser)
{
    traverser->pushStack(localTransform);
    setTransform(traverser->getStackTop());
    event_visited.fire(traverser);
}

void CTransform::visitWithoutEvent(engine::scene::TreeTraverser* traverser)
{
    traverser->pushStack(localTransform);
    setTransform(traverser->getStackTop());
}

void CTransform::addChild(engine::components::id_t newChild)
{
    children.push_back(newChild);

    event_transformCompAttached.fire(newChild);
}

void CTransform::removeChild(engine::components::id_t childToRemove)
{
    for (int i = 0; i < children.size(); ++i)
    {
        if(children[i] == childToRemove)
        {
            children[i] = children.back();
            children.pop_back();
            return;
        }
    }
}

void CTransform::translate(glm::vec3 translation)
{
    localTransform = glm::translate(localTransform, translation);
}

void CTransform::rotate(float angleInDegree, glm::vec3 axis)
{
    localTransform = glm::rotate(localTransform, angleInDegree, axis);
}

void CTransform::setTransform(const glm::mat4 newTransform)
{
    transform = newTransform;
}

void CTransform::setLocalTransform(const glm::mat4 newTransform)
{
    localTransform = newTransform;
}

glm::mat4 CTransform::getLocalTransform()
{
    return localTransform;
}

glm::mat4 CTransform::getTransform()
{
    return transform;
}

glm::vec3 CTransform::getForward() const
{
    const glm::mat4 inverted = glm::inverse(transform);
    const glm::vec3 forward = normalize(glm::vec3(inverted[2]));
    return forward;
}

const std::vector<engine::components::id_t>& CTransform::getChildren() const
{
    return children;
}
