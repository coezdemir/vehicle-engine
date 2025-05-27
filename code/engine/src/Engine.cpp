// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#include <engine/Engine.h>

engine::Engine::Engine() : bSkippedFirstTick(false)
{

}

void engine::Engine::run()
{
    // Start up the systems
    event_startup.fire(this);

    context.event_beginPlay.subscribe([this]() {
        event_beginPlay.fire(this);
    });

    context.event_newFrame.subscribe([this]() {
       event_newFrame.fire();
    });

    context.event_draw.subscribe([this]() {
        event_draw.fire();
    });

    event_afterStartup.fire(this);

    try
    {
        // Enter the run loop
        context.run([this](context::Context* c) {
            tick(c);
        });
    }
    catch (std::exception& e)
    {
        std::cerr << "[Engine::run: Uncaught Exception] ----------------- " << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "[Shutdown]" << std::endl;
    }

    // Shutdown everything else before we shutdown the system
    event_beforeShutdown.fire(this);

    // Shut down the systems
    event_shutdown.fire(this);
}

// inside namespace engine
void engine::Engine::tick(context::Context* c)
{
    if(SKIP_FIRST_TICK && !bSkippedFirstTick)
    {
        bSkippedFirstTick = true;
    }
    else
    {
        event_tick.fire(c->getDeltaTime());
    }
}

engine::entities::Entity engine::Engine::getEntity(entities::id_t id)
{
    return entities.getEntity(id);
}

void engine::Engine::printInstance()
{
    printf("I am an engine instance.\n");
}

engine::context::Context& engine::Engine::getContext()
{
    return context;
}
