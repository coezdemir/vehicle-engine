// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#include <engine/world/World.h>
#include <engine/world/AActor.h>

engine::world::World::World(engine::Engine* engine) : engine(engine)
{
    handle_engineTick = engine->event_tick.subscribe([this](const float deltaTime) {
        event_worldTick.fire(deltaTime);
    });

    engine->event_beforeShutdown.subscribe([this](engine::Engine* engine) {
        shutdown();
    });

    engine::context::Context& context = engine->getContext();

    handle_engineEndFrame = context.event_endFrame.subscribe([this]() {
        destroyMarkedActors();
    });

    handle_contextKeyInput = context.event_keyInput.subscribe(
            [this](const GLFWwindow* const w, int k, int s, int a, int m) {
                event_keyInput.fire(w, k, s, a, m);
            }
    );

    handle_contextControllerInput = context.event_controllerInput.subscribe(
            [this](const int controllerID, const GLFWgamepadstate& state) {
                event_controllerInput.fire(controllerID, state);
            }
    );

    handle_contextMouseMove = context.event_mouseMove.subscribe(
            [this](const double xPos, const double yPos) {
                event_mouseMove.fire(xPos, yPos);
            }
    );

    handle_contextMouseInput = context.event_mouseInput.subscribe(
            [this](const int key, const int action, const int mode) {
                event_mouseInput.fire(key, action, mode);
            }
    );;

    handle_contextMouseScroll = context.event_mouseScroll.subscribe(
            [this](const double xOffset, const double yOffset) {
                event_mouseScroll.fire(xOffset, yOffset);
            }
    );

    handle_contextMouseCatch = context.event_mouseCatch.subscribe(
            [this](GLFWwindow* glfWwindow, const bool bEntered) {
                event_mouseCatch.fire(glfWwindow, bEntered);
            }
    );
}

engine::world::World::~World()
{
    engine->event_tick.unsubscribe(handle_engineTick);
    engine->getContext().event_endFrame.unsubscribe(handle_engineEndFrame);
    engine->getContext().event_keyInput.unsubscribe(handle_contextKeyInput);
    // Destruction of world results in destruction of Actors
    // Note: This must happen before subsystem shutdown
}

void engine::world::World::destroyActor(AActor* actor)
{
    actorsToDestroy.push_back(actor);
}

engine::entities::Entity engine::world::World::createEntity()
{
    return engine->entities.createEntity();
}

void engine::world::World::destroyEntity(entities::Entity entity)
{
    return engine->entities.releaseEntity(entity);
}

void engine::world::World::destroyMarkedActors()
{
    for (int i = actorsToDestroy.size() - 1; i >= 0; --i)
    {
        destroyEntity(actorsToDestroy[i]->getRootEntity());

        for (int j = 0; j < actors.size(); j++)
        {
            if (actorsToDestroy[i] == actors[j].get())
            {
                actors[j] = actors[actors.size() - 1];
                actors.pop_back();

                break;
            }
        }
    }

    actorsToDestroy.clear();
}

void engine::world::World::shutdown()
{
    for (int i = actors.size() - 1; i >= 0; --i)
        destroyEntity(actors[i]->getRootEntity());

    actors.clear();
}
