// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#include <engine/entities/Entity.h>
#include <engine/entities/EntityPool.h>
#include <engine/Engine.h>

engine::entities::Entity::Entity(engine::entities::EntityPool* pool, engine::Engine* engine, engine::entities::id_t id)
        : id(id), engine(engine), pool(pool)
{

}

bool engine::entities::Entity::isValid() const
{
    return pool->isAlive(id);
}