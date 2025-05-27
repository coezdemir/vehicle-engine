//
// Created by CemOe on 17.08.2021.
//

#include <engine/renderer/components/CModel.h>
#include <engine/scene/TreeTraverser.h>
#include <engine/renderer/Renderer.h>

using namespace engine::renderer::components;

CModel::CModel(const engine::components::id_t id, const engine::entities::id_t owner)
    : CComponent(id, owner),
        bIsVisible(true)
{
}

CModel::~CModel()
{
    manager::ModelManager::lostUser(mesh);
}

void CModel::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* meshConstruction =
            dynamic_cast<const engine::renderer::components::CModel::ConstructionInfo*>(constructionInfo);
    assert(meshConstruction && "Please use the correct ConstructionInfo: CCamera::ConstructionInfo");

    mesh = manager::ModelManager::loadModel(meshConstruction->path);
    bFlunctuateEmission = meshConstruction->bFlunctuateEmission;
    flunctuationInterval = meshConstruction->flunctuationInterval;
    flunctuationDelay = meshConstruction->flunctuationDelay;
}

void CModel::changeShader(const engine::renderer::Shader* newShader)
{
    shader = newShader;
}

bool CModel::isVisible() const
{
    return bIsVisible;
}

void CModel::setVisibility(const bool bNewVisibility)
{
    bIsVisible = bNewVisibility;
}

void CModel::setModel(std::string_view newMeshPath)
{
    manager::ModelManager::lostUser(mesh);
    mesh = manager::ModelManager::loadModel(newMeshPath);
}

void CModel::getAllVertices(std::vector<std::vector<glm::vec3>>& allVertices)
{
    mesh->getAllVertices(allVertices);
}

void CModel::getAllIndices(std::vector<std::vector<unsigned int>>& allIndices)
{
    mesh->getAllIndices(allIndices);
}

void CModel::getAllTriangleCounts(std::vector<unsigned int>& allTriangleCounts)
{
    mesh->getAllTriangleCounts(allTriangleCounts);
}

void CModel::initialVisit(scene::TreeTraverser* traverser)
{

}

void CModel::visit(scene::TreeTraverser* traverser)
{
    if(!bIsVisible)
        return;

    glm::mat4 view = traverser->getView();
    glm::mat4 stackTop = traverser->getStackTop();

    shader->use();
    shader->setMatrix("model", stackTop);
    shader->setBool("material.bFlunctuateEmission", bFlunctuateEmission);
    shader->setFloat("material.flunctuationInterval", flunctuationInterval);
    shader->setFloat("material.flunctuationDelay", flunctuationDelay);

    mesh->draw(shader);
}
