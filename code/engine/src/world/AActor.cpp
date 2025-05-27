// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#include <engine/world/AActor.h>

engine::world::AActor::AActor(World* const world) : world(world), rootEntity(world->createEntity()), bIsAlive(true)
{
    handle_worldTick = world->event_worldTick.subscribe([this](float deltaTime) {
        tick(deltaTime);
    });
}

void engine::world::AActor::tick(const float deltaTime)
{

}

bool engine::world::AActor::isAlive() const
{
    return bIsAlive;
}
