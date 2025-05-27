//
// Created by Cem on 01.01.2022.
//
#pragma once
#include <map>
#include <engine/Engine.h>
#include <engine/ui/windows/WWindow.h>
#include "Gamemode.h"
#include "../windows/WStartingWindow.h"
#include "../windows/WScoreWindow.h"
#include "../windows/WItemWindow.h"
#include "../windows/WResultsWindow.h"

namespace game
{
    class Game;
}

namespace game::gamemode
{
    class PointGamemode : public game::gamemode::Gamemode
    {
        friend class game::Game;

        explicit PointGamemode(float timeLimit, engine::Engine* engine);

    public:
        struct PGUpdateData : public Gamemode::UpdateData
        {
            world::ACar* updatingActor;

            int points = 0;
        };

        void start() override;

        void restart(float newTime);

        void tick(float deltaTime);

        void update(const UpdateData* data) override;

        void registerPlayer(world::ACar* player) override;

        void reset() override;

        [[nodiscard]] float getTime() const;

        [[nodiscard]] int getScore(world::ACar* player) const;

    protected:
        void fireFinishEvent() override;

        float time;

        std::map<world::ACar*, int> currScores;

        bool bIsFinished;

        engine::Engine* engine;

        std::shared_ptr<engine::ui::windows::WWindow> startingWindow;

        std::vector<std::shared_ptr<engine::ui::windows::WWindow>> scoreWindows;

        std::vector<std::shared_ptr<engine::ui::windows::WWindow>> itemWindows;

        std::shared_ptr<engine::ui::windows::WWindow> resultsWindow;

        std::shared_ptr<engine::ui::windows::WWindow> circuitEndWindow;

        std::vector<std::shared_ptr<engine::ui::windows::WWindow>> timerWindows;

        std::vector<int> overallScores;

        bool bIsAlreadyInitialized;

        int currentTrack;
    };
}
