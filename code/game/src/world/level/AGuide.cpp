//
// Created by Cem on 19.07.2022.
//
#include "AGuide.h"

game::world::AGuide::AGuide(glm::mat4 transform, engine::world::World* world)
        : AMesh(transform, world, "game/guides/single-guide.obj")
{
    engine::renderer::components::CModel::ConstructionInfo meshInfo;
    meshInfo.instantiator = this;
    meshInfo.path = "game/guides/single-guide.obj";
    meshInfo.bFlunctuateEmission = true;
    meshInfo.flunctuationInterval = 2.f;
    meshInfo.flunctuationDelay = 0.f;
    meshComp->init(&meshInfo);

    engine::scene::components::CTransform::ConstructionInfo transformInfo;
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-12.5f, 0.f, 0.f));
        transformComp2 = rootEntity.addComponent<engine::scene::components::CTransform>();
        transformInfo.instantiator = this;
        transformInfo.transform = transform;
        transformComp2->init(&transformInfo);
        transformComp->addChild(transformComp2->getId());

        meshComp2 = rootEntity.addComponent<engine::renderer::components::CModel>();
        meshInfo.flunctuationDelay = -1.f;
        meshComp2->init(&meshInfo);
        transformComp2->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
            meshComp2->visit(traverser);
        });
    }

    {
        transform = glm::translate(transform, glm::vec3(-12.5f, 0.f, 0.f));
        transformComp3 = rootEntity.addComponent<engine::scene::components::CTransform>();
        transformInfo.transform = transform;
        transformComp3->init(&transformInfo);
        transformComp->addChild(transformComp3->getId());

        meshComp3 = rootEntity.addComponent<engine::renderer::components::CModel>();
        meshInfo.flunctuationDelay -= 1.f;
        meshComp3->init(&meshInfo);
        transformComp3->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
            meshComp3->visit(traverser);
        });
    }

    {
        transform = glm::translate(transform, glm::vec3(-12.5f, 0.f, 0.f));
        transformComp4 = rootEntity.addComponent<engine::scene::components::CTransform>();
        transformInfo.transform = transform;
        transformComp4->init(&transformInfo);
        transformComp->addChild(transformComp4->getId());

        meshComp4 = rootEntity.addComponent<engine::renderer::components::CModel>();
        meshInfo.flunctuationDelay -= 1.f;
        meshComp4->init(&meshInfo);
        transformComp4->event_visited.subscribe([this](engine::scene::TreeTraverser* const traverser) {
            meshComp4->visit(traverser);
        });
    }
}
