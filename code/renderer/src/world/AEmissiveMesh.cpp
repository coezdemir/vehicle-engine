//
// Created by Cem on 20.07.2022.
//

#include <engine/renderer/world/AEmissiveMesh.h>

engine::renderer::world::AEmissiveMesh::AEmissiveMesh(glm::mat4 transform, engine::world::World* world,
                                                      engine::renderer::components::CModel::ConstructionInfo& constructionInfo)
                                                      : AMesh(transform, world, constructionInfo.path)
{
    constructionInfo.instantiator = this;
    meshComp->init(&constructionInfo);
}
