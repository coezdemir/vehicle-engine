//
// Created by CemOe on 22.08.2021.
//
#include <engine/entities/EntityPool.h>

engine::entities::EntityPool::EntityPool(engine::Engine* engine)
    : engine(engine), activeEntities(std::set<id_t>())
{

}

engine::entities::Entity engine::entities::EntityPool::getEntity(engine::entities::id_t id)
{
    auto search = activeEntities.find(id);
    if (search != activeEntities.end())
    {
        return Entity(this, engine, id);
    }
    else
    {
        throw std::runtime_error("No such entity!");
    }
}

bool engine::entities::EntityPool::isAlive(engine::entities::id_t id)
{
    if(activeEntities.empty())
        return false;

    auto search = activeEntities.find(id);
    return (search != activeEntities.end());
}

engine::entities::Entity engine::entities::EntityPool::createEntity()
{
    id_t id = ++next_id;
    activeEntities.insert(id);
    auto e = Entity(this, engine, id);
    event_createdEntity.fire(e);
    return e;
}

void engine::entities::EntityPool::releaseEntity(engine::entities::Entity entity)
{
    event_destroyedEntity.fire(entity);
    activeEntities.erase(entity.id);
}
