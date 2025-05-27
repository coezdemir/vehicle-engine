//
// Created by Cem on 01.01.2022.
//
#pragma once
#include <memory>
#include <vector>
#include <engine/callbacks/Event.h>

namespace game
{
    namespace world
    {
        class ACar;
    }

    namespace gamemode
    {
        class Gamemode
        {
        public:
            /* Events */
            using event_t = typename engine::callbacks::Event<Gamemode>;

            struct UpdateData
            {
                virtual ~UpdateData() = default;
            };

            virtual void start() = 0;

            virtual void reset()
            {
                players.clear();
            }

            virtual void registerPlayer(world::ACar* player)
            {
                players.push_back(player);
            }

            [[nodiscard]] virtual unsigned int getNumberOfPlayers() const
            {
                return players.size();
            }

            [[nodiscard]] const std::vector<world::ACar*>& getPlayers() const
            {
                return players;
            }

            virtual void update(const UpdateData* data) = 0;

            event_t event_started;

            event_t event_finished;

            event_t event_resultsSpawned;

        protected:
            virtual void fireStartEvent()
            {
                event_started.fire();
            }

            virtual void fireFinishEvent()
            {
                event_finished.fire();
            }

            virtual void fireSpawnedResultsEvent()
            {
                event_resultsSpawned.fire();
            }

            std::vector<world::ACar*> players;
        };
    }
}
