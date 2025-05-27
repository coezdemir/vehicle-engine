// Based on GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#pragma once
#include <engine/Engine_NS.h>
#include <engine/callbacks/Event.h>

namespace engine::components
{
    struct ConstructionInfo
    {
        engine::world::AActor* instantiator = nullptr;

        virtual ~ConstructionInfo() = default;
    };

    /***
     * A component contains data used by a system.
     */
    struct CComponent
    {
    public:
        template <typename, size_t>
        friend
        class ComponentPool;

        explicit CComponent(id_t id = components::invalid_id, entities::id_t owner = entities::invalid_id);

        virtual ~CComponent() = default;

        CComponent(const CComponent&) = delete;

        virtual void init(const ConstructionInfo* constructionInfo);

        virtual void tick(float deltaTime);

        [[nodiscard]] bool isDeleted() const;

        [[nodiscard]] engine::world::AActor* getInstantiator() const;

        [[nodiscard]] engine::entities::id_t getOwner() const;

        [[nodiscard]] engine::components::id_t getId() const;

    protected:
        entities::id_t owner = entities::invalid_id;

        id_t id = components::invalid_id;

        bool deleted = true;

        engine::world::AActor* instantiator;
    };
}
