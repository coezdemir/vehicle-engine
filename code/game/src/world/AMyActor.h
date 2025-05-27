//
// Created by CemOe on 08.10.2021.
//
#pragma once
#include <engine/world/AActor.h>
#include <engine/scene/components/CTransform.h>

namespace game::world
{
    class AMyActor : public engine::world::AActor
    {
    protected:
        engine::scene::components::transform_handle_t transformComp;

    private:
        engine::components::id_t transformID = engine::components::invalid_id;

        engine::scene::components::transform_handle_t parentTransform;

    public:
        AMyActor(const glm::mat4 transform, engine::world::World* const world)
                : AActor(world)
        {
            // Init transform component
            transformComp = getRootEntity().addComponent<engine::scene::components::CTransform>();
            engine::scene::components::CTransform::ConstructionInfo transformInfo;
            transformInfo.instantiator = this;
            transformInfo.transform = transform;
            transformComp->init(&transformInfo);
        }

        [[nodiscard]] glm::vec4 getLocation()
        {
            if (transformID == engine::components::invalid_id)
                return glm::vec4(0.f);

            return getRootEntity().getComponent<engine::scene::components::CTransform>(
                    transformID)->getTransform()[3];
        }

        void destroy() override
        {
            if (parentTransform.isValid())
                parentTransform->removeChild(transformComp->getId());

            AActor::destroy();
        }

        void attachActor(AMyActor* otherActor)
        {
            transformComp->addChild(otherActor->transformComp->getId());

            otherActor->parentTransform = transformComp;
        }

        [[nodiscard]] const engine::scene::components::transform_handle_t& getTransformComp() const
        {
            return transformComp;
        }

        virtual glm::mat4 getLocalTransform()
        {
            return transformComp->getLocalTransform();
        }

        void attachTo(engine::scene::components::transform_handle_t transformComponent)
        {
            transformComponent->addChild(transformComp->getId());

            parentTransform = transformComponent;
        }
    };
}