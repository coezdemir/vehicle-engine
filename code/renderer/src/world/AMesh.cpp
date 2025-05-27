//
// Created by s369340 on 18.07.2022.
//

#include "engine/renderer/world/AMesh.h"

engine::renderer::world::AMesh::AMesh(glm::mat4 transform, engine::world::World* world, std::string_view filename)
        : AActor(world)
{
    // Init transform component
    transformComp = rootEntity.addComponent<engine::scene::components::CTransform>();
    scene::components::CTransform::ConstructionInfo transformInfo;
    transformInfo.instantiator = this;
    transformInfo.transform = transform;
    transformComp->init(&transformInfo);

    meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
    renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = filename;
    meshComp->init(&meshInfo);

    transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser) {
        meshComp->visit(traverser);
    });
}
