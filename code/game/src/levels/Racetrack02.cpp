//
// Created by CemOe on 04.12.2021.
//

#include "Racetrack02.h"
#include <engine/renderer/world/AEmissiveMesh.h>
#include <engine/renderer/world/ACinematicCamera.h>
#include "../world/level/AItem.h"
#include "../world/level/ARespawnPlane.h"
#include "../world/level/AGoal.h"
#include "../world/mesh/AMovingMesh.h"
#include "../world/obstacles/ABumper.h"
#include "../world/obstacles/AMovingBumper.h"
#include "../world/level/AGuide.h"
#include "../world/level/ABooster.h"

void game::level::Racetrack02::loadLevel(gamemode::Gamemode* gamemode)
{
    printf("-- Loading Racetrack02.\n");
    printf("\n *** HERE THE ACTORS ARE CREATED *** \n\n");
    glm::mat4 transform = glm::mat4(1.f);

    fireStartedLoadingEvent();

    // Light
    transform = glm::mat4(1.f);
    lightActor = world->createActor<engine::renderer::world::ALight>(transform, world);
    actors.push_back(lightActor);

    // Respawn Plane
    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(650.f, -5.f, -200.f));
    transform = glm::scale(transform, glm::vec3(1750.f, 10.f, 1250.f));
    actors.push_back(world->createActor<world::ARespawnPlane>(transform, world));

    // Guides
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-17.5f, 7.5f, 200.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(350.0f, 7.5f, 167.5f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(282.5f, 7.5f, -200.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(650.f, 7.5f, -167.5f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(582.5f, 7.5f, 200.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1300.f, 7.5f, 167.5f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1267.5f, 7.5f, -550.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-50.f, 7.5f, -517.5f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));
    }

    std::shared_ptr<world::ACheckpoint> lastCheckpoint;
    // Checkpoints
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(700.f, 0.f, 150.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1100.f, 0.f, 150.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1200.f, 0.f, -500.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(100.f, 0.f, -500.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        lastCheckpoint = world->createActor<world::ACheckpoint>(transform, world);
        actors.push_back(lastCheckpoint);
    }


    // Goal
    {
        transform = glm::mat4(1.f);
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        goalActor = world->createActor<world::AGoal>(transform, world, lastCheckpoint.get());
        goalActor->event_goalReached.subscribe([gamemode](world::ACar* car) {
            gamemode::PointGamemode::PGUpdateData updateData;
            updateData.updatingActor = car;
            updateData.points = 1000;
            gamemode->update(&updateData);
        });
        actors.push_back(goalActor);

        transform = glm::mat4(1.f);
        engine::renderer::components::CModel::ConstructionInfo meshInfo;
        meshInfo.path = "game/goal/goal.obj";
        meshInfo.bFlunctuateEmission = true;
        meshInfo.flunctuationInterval = 2.f;
        meshInfo.flunctuationDelay = 0.f;
        actors.push_back(world->createActor<engine::renderer::world::AEmissiveMesh>(transform, world, meshInfo));
    }

    // Item Boxes
    {
        // First item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(150.f, 4.f, 150.f));
            transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(-30.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(-10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
        }

        // Second item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(750.f, 4.f, 150.f));
            transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(-30.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(-10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
        }

        // Third item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(1250.f, 4.f, -200.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(-30.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(-10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
        }

        // Fourth item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(250.f, 4.f, -500.f));
            transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(-30.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));

            transform = glm::translate(transform, glm::vec3(-10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
        }
    }

    // Bumper obstacles
    {
        // At the end before the goal
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(0.f, 3.f, -100.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));

            transform = glm::translate(transform, glm::vec3(12.f, 0.f, -50.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));

            transform = glm::translate(transform, glm::vec3(-24.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));

            transform = glm::translate(transform, glm::vec3(0.f, 0.f, -150.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));

            transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));

            transform = glm::translate(transform, glm::vec3(-12.f, 0.f, -50.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));
        }

        // After the second wall
        {
            transform = glm::translate(transform, glm::vec3(450.f, 0.f, -162.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));

            transform = glm::translate(transform, glm::vec3(50.f, 0.f, 24.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));
        }

        // After the first wall
        {
            transform = glm::translate(transform, glm::vec3(200.f, 0.f, -24.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));

            transform = glm::translate(transform, glm::vec3(50.f, 0.f, 24.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));
        }

        // Before the first wall
        {
            transform = glm::translate(transform, glm::vec3(200.f, 0.f, -24.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));

            transform = glm::translate(transform, glm::vec3(50.f, 0.f, 24.f));
            actors.push_back(world->createActor<world::ABumper>(transform, world));
        }

        // After the ramp the moving bumpers
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(1250.f, 3.f, 50.f));
            transform = glm::translate(transform, glm::vec3(12.f, 0.f, 0.f));
            engine::scene::components::CTranslation::ConstructionInfo translationInfo;
            translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) - glm::vec3(24.f, 0.f, 0.f)};
            translationInfo.speed = 13.f;
            translationInfo.bShouldReverse = true;
            actors.push_back(world->createActor<world::AMovingBumper>(transform, world, &translationInfo));

            transform = glm::translate(transform, glm::vec3(-24.f, 0.f, -50.f));
            translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) + glm::vec3(24.f, 0.f, 0.f)};
            actors.push_back(world->createActor<world::AMovingBumper>(transform, world, &translationInfo));

            transform = glm::translate(transform, glm::vec3(24.f, 0.f, -100.f));
            translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) - glm::vec3(24.f, 0.f, 0.f)};
            actors.push_back(world->createActor<world::AMovingBumper>(transform, world, &translationInfo));

            transform = glm::translate(transform, glm::vec3(-24.f, 0.f, -50.f));
            translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) + glm::vec3(24.f, 0.f, 0.f)};
            actors.push_back(world->createActor<world::AMovingBumper>(transform, world, &translationInfo));

            transform = glm::translate(transform, glm::vec3(24.f, 0.f, -150.f));
            translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) - glm::vec3(24.f, 0.f, 0.f)};
            actors.push_back(world->createActor<world::AMovingBumper>(transform, world, &translationInfo));

            transform = glm::translate(transform, glm::vec3(-24.f, 0.f, -50.f));
            translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) + glm::vec3(24.f, 0.f, 0.f)};
            actors.push_back(world->createActor<world::AMovingBumper>(transform, world, &translationInfo));

            transform = glm::translate(transform, glm::vec3(24.f, 0.f, -50.f));
            translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) - glm::vec3(24.f, 0.f, 0.f)};
            actors.push_back(world->createActor<world::AMovingBumper>(transform, world, &translationInfo));
        }
    }

    // Wall obstacles
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(600.f, 13.f, -525.f));
        engine::scene::components::CTranslation::ConstructionInfo translationInfo;
        translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) + glm::vec3(0.f, 0.f, 50.f)};
        translationInfo.speed = 13.f;
        translationInfo.bShouldReverse = true;
        actors.push_back(
                world->createActor<world::AMovingMesh>(transform, world, "game/obstacles/wall/SmallWall.obj", translationInfo));

        transform = glm::translate(transform, glm::vec3(250.f, 0.f, 50.f));
        translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) - glm::vec3(0.f, 0.f, 50.f)};
        translationInfo.speed = 11.f;
        actors.push_back(
                world->createActor<world::AMovingMesh>(transform, world, "game/obstacles/wall/SmallWall.obj", translationInfo));
    }

    // Booster
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(70.f, 2.5f, 165.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(280.f, 2.5f, 60.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(415.f, 2.5f, -130.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(580.f, 2.5f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1250.f, 2.5f, 25.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1250.f, 2.5f, -125.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1250.f, 2.5f, -330.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1125.f, 2.5f, -520.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(790.f, 2.5f, -500.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(555.f, 2.5f, -500.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(120.f, 2.5f, -520.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(0.f, 2.5f, -300.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(0.f, 2.5f, -60.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));
    }

    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(675.f, 0.f, -200.f));
    glm::mat4 offset = glm::mat4(1.f);
    offset = glm::translate(offset, glm::vec3(0.f, 500.f, -1300.f));
    actors.push_back(world->createActor<engine::renderer::world::ACinematicCamera>(transform, offset, world));

    fireFinishedLoadingEvent();

    //dynamic_cast<gamemode::PointGamemode*>(gamemode)->start();
    dynamic_cast<gamemode::PointGamemode*>(gamemode)->restart(600);
}