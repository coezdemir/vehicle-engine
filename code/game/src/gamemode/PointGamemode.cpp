//
// Created by Cem on 01.01.2022.
//

#include "PointGamemode.h"
#include <engine/ui/UserInterface.h>
#include "../windows/WTimerWindow.h"
#include "../world/ACar.h"
#include "../windows/WCircuitEnd.h"

game::gamemode::PointGamemode::PointGamemode(float timeLimit, engine::Engine* engine)
        : time(timeLimit), engine(engine), bIsAlreadyInitialized(false)
{
    bIsFinished = true;
}

void game::gamemode::PointGamemode::start()
{
    currentTrack = 1;

    auto* userInterfaceSystem = engine->getSystem<engine::ui::UserInterface>();

    startingWindow = userInterfaceSystem->addWindow<game::ui::windows::WStartingWindow>(
            &userInterfaceSystem->nkContext, engine->getContext().getWindow());

    // Add player windows and start the game
    std::dynamic_pointer_cast<game::ui::windows::WStartingWindow>(
            startingWindow)->event_continue.subscribe([this, userInterfaceSystem]() {
                                                          if (!bIsAlreadyInitialized)
                                                          {
                                                              for (int i = 0; i < getNumberOfPlayers(); ++i)
                                                              {
                                                                  scoreWindows.push_back(
                                                                          userInterfaceSystem->addWindow<game::ui::windows::WScoreWindow>(&userInterfaceSystem->nkContext,
                                                                                                                                          engine->getContext().getWindow(),
                                                                                                                                          players[i], this)
                                                                  );

                                                                  itemWindows.push_back(
                                                                          userInterfaceSystem->
                                                                                  addWindow<game::ui::windows::WItemWindow>(&userInterfaceSystem->nkContext,
                                                                                                                            engine->getContext().getWindow(), players[i],
                                                                                                                            this)
                                                                  );

                                                                  timerWindows.push_back(
                                                                          userInterfaceSystem->
                                                                                  addWindow<game::ui::windows::WTimerWindow>(&userInterfaceSystem->nkContext,
                                                                                                                             engine->getContext().getWindow(), players[i],
                                                                                                                             this)
                                                                  );
                                                              }

                                                              bIsAlreadyInitialized = true;
                                                          } else
                                                          {
                                                              for (int i = 0; i < getNumberOfPlayers(); ++i)
                                                              {
                                                                  userInterfaceSystem->readdWindowToDrawList(scoreWindows[i]);
                                                                  userInterfaceSystem->readdWindowToDrawList(itemWindows[i]);
                                                                  userInterfaceSystem->readdWindowToDrawList(timerWindows[i]);
                                                              }
                                                          }

                                                          userInterfaceSystem->removeWindowFromDrawList(startingWindow);

                                                          fireStartEvent();

                                                          bIsFinished = false;
                                                      }
    );


    for (int i = 0; i < getNumberOfPlayers(); ++i)
    { overallScores.emplace_back(0); }

}

void game::gamemode::PointGamemode::restart(const float newTime)
{
    time = newTime;
    bIsFinished = false;
    ++currentTrack;

    auto* userInterfaceSystem = engine->getSystem<engine::ui::UserInterface>();

    for (int i = 0; i < getNumberOfPlayers(); i++)
    {
        currScores[players[i]] = 0;
        userInterfaceSystem->readdWindowToDrawList(scoreWindows[i]);
        userInterfaceSystem->readdWindowToDrawList(itemWindows[i]);
        userInterfaceSystem->readdWindowToDrawList(timerWindows[i]);
    }
}

void game::gamemode::PointGamemode::update(const game::gamemode::Gamemode::UpdateData* data)
{
    if (bIsFinished)
        return;

    auto* updateData = dynamic_cast<const game::gamemode::PointGamemode::PGUpdateData*>(data);

    assert(updateData
           && "Please use the correct update data struct: game::gamemode::PointGamemode::PGUpdateData");

    currScores[updateData->updatingActor] += updateData->points;
}

void game::gamemode::PointGamemode::tick(float deltaTime)
{
    if (!bIsFinished)
    {
        time -= deltaTime;

        if (time <= 0.f && !bIsFinished)
            fireFinishEvent();
    }
}

void game::gamemode::PointGamemode::registerPlayer(world::ACar* player)
{
    player->setID(players.size());

    Gamemode::registerPlayer(player);

    currScores.insert({player, 0});

    player->event_respawned.subscribe([this, player](world::ACar* lastCollidedCar) {
        if (lastCollidedCar)
        {
            gamemode::PointGamemode::PGUpdateData updateData;
            updateData.updatingActor = lastCollidedCar;
            updateData.points = 600;

            update(&updateData);
            player->setLastCollidedCar(nullptr);
        }
    });
}

int game::gamemode::PointGamemode::getScore(world::ACar* player) const
{
    return currScores.at(player);
}

void game::gamemode::PointGamemode::fireFinishEvent()
{
    bIsFinished = true;

    auto* userInterfaceSystem = engine->getSystem<engine::ui::UserInterface>();

    std::vector<int> results;
    for (int i = 0; i < getNumberOfPlayers(); ++i)
    {
        userInterfaceSystem->removeWindowFromDrawList(scoreWindows[i]);
        userInterfaceSystem->removeWindowFromDrawList(itemWindows[i]);
        userInterfaceSystem->removeWindowFromDrawList(timerWindows[i]);
        results.push_back(currScores[players[i]]);
        overallScores[i] += currScores[players[i]];
    }

    if (currentTrack % 3)
    {
        resultsWindow = userInterfaceSystem->addWindow<game::ui::windows::WResultsWindow>(
                &userInterfaceSystem->nkContext,
                engine->getContext().getWindow(),
                results);


        std::dynamic_pointer_cast<game::ui::windows::WResultsWindow>(resultsWindow)->
                event_continue.subscribe([this, userInterfaceSystem]() {
                                             userInterfaceSystem->removeWindowFromDrawList(resultsWindow);
                                             resultsWindow.reset();
                                             Gamemode::fireFinishEvent();
                                         }
        );
    } else
    {
        circuitEndWindow = userInterfaceSystem->addWindow<game::ui::windows::WCircuitEnd>(
                &userInterfaceSystem->nkContext,
                engine->getContext().getWindow(),
                overallScores);

        std::dynamic_pointer_cast<game::ui::windows::WCircuitEnd>(circuitEndWindow)->
                event_continue.subscribe([this, userInterfaceSystem]() {
                                             userInterfaceSystem->removeWindowFromDrawList(circuitEndWindow);
                                             circuitEndWindow.reset();

                                             for (auto& score: overallScores)
                                                 score = 0;

                                             Gamemode::fireFinishEvent();
                                         }
        );
    }


    fireSpawnedResultsEvent();
}

void game::gamemode::PointGamemode::reset()
{
    for (world::ACar* player: players)
        currScores[player] = 0;

    Gamemode::reset();
}

float game::gamemode::PointGamemode::getTime() const
{
    return time;
}

