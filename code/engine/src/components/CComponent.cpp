//
// Created by CemOe on 22.08.2021.
//
#include <engine/components/CComponent.h>
#include <engine/world/AActor.h>

engine::components::CComponent::CComponent(const id_t id, const entities::id_t owner)
        : id(id), owner(owner)
{

}

void engine::components::CComponent::tick(const float deltaTime)
{

}

engine::entities::id_t engine::components::CComponent::getOwner() const
{
    return owner;
}

engine::components::id_t engine::components::CComponent::getId() const
{
    return id;
}

bool engine::components::CComponent::isDeleted() const
{
    return deleted;
}

void engine::components::CComponent::init(const ConstructionInfo* const constructionInfo)
{
    assert(constructionInfo->instantiator && "instantiator == nullptr");

    instantiator = constructionInfo->instantiator;
}

engine::world::AActor* engine::components::CComponent::getInstantiator() const
{
    return instantiator;
}