//
// Created by CemOe on 02.12.2021.
//

#include "Racetrack01.h"
#include <engine/renderer/world/ACinematicCamera.h>
#include <engine/renderer/world/AEmissiveMesh.h>
#include <engine/renderer/world/AMesh.h>
#include "../world/level/AItem.h"
#include "../world/level/ARespawnPlane.h"
#include "../world/level/AGoal.h"
#include "../world/obstacles/ABumper.h"
#include "../world/obstacles/AMovingBumper.h"
#include "../world/level/AGuide.h"
#include "../world/level/ABooster.h"

void game::level::Racetrack01::loadLevel(gamemode::Gamemode* gamemode)
{
    printf("-- Loading Racetrack01.\n");
    printf("\n *** HERE THE ACTORS ARE CREATED *** \n\n");

    fireStartedLoadingEvent();

    glm::mat4 transform = glm::mat4(1.f);

    // Light
    transform = glm::mat4(1.f);
    lightActor = world->createActor<engine::renderer::world::ALight>(transform, world);
    actors.push_back(lightActor);

    // Respawn Plane
    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(-200.f, -5.f, 350.f));
    transform = glm::scale(transform, glm::vec3(1250.f, 10.f, 1250.f));
    actors.push_back(world->createActor<world::ARespawnPlane>(transform, world));

    // Guides
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(17.5f, 7.5f, 350.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-300.f, 7.5f, 317.5f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-232.5f, 7.5f, -50.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-650.f, 7.5f, -17.5f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-617.5f, 7.5f, 750.0f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(200.0f, 7.5f, 717.5f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(167.5f, 7.5f, -100.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));
    }

    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0.f, 0.f, 25.f));
    engine::scene::components::CTranslation::ConstructionInfo translationInfo;
    translationInfo.speed = 25.f;
    translationInfo.pointsToMoveBetween = std::pair<glm::vec3, glm::vec3>(glm::vec3(0.f, 0.f, 20.f), glm::vec3(0.f, 0.f, 25.f));
    translationInfo.bShouldReverse = true;
    translationInfo.cooldownTimeSec = 1.f;
    actors.push_back(world->createActor<world::AMovingBumper>(transform, world, &translationInfo));

    std::shared_ptr<world::ACheckpoint> lastCheckpoint;
    // Checkpoints
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-600.f, 0.f, 25.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-550.f, 0.f, 700.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-100.f, 0.f, 700.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(150.f, 0.f, 300.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
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
            transform = glm::translate(transform, glm::vec3(0.f, 4.f, 150.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(-30.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(-10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world))   ;
        }

        // Second item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(-250.f, 4.f, 150.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(-30.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(-10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world))  ;
        }

        // Third item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(-600.f, 4.f, 50.f));
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
            transform = glm::translate(transform, glm::vec3(-600.f, 4.f, 600.f));
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

        // Fifth item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(150.f, 4.f, 50.f));
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

        // Sixth item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(150.f, 4.f, 600.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(-30.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world));
            transform = glm::translate(transform, glm::vec3(-10.f, 0.f, 0.f));
            actors.push_back(world->createActor<world::AItem>(transform, world))       ;
        }
    }

    // Bumper obstacles
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-612.f, 3.f, 100.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(138.f, 3.f, 100.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(24.f, 0.f, 0.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));

        transform = glm::translate(transform, glm::vec3(-12.f, 0.f, 50.f));
        actors.push_back(world->createActor<world::ABumper>(transform, world));
    }

    // Booster
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-50.f, 2.5f, 315.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-320.f, 2.5f, 20.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-475.f, 2.5f, -20.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-600.f, 2.5f, 200.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-600.f, 2.5f, 400.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(150.f, 2.5f, 500.f));
        transform = glm::rotate(transform, glm::radians(-180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(150.f, 2.5f, 300.f));
        transform = glm::rotate(transform, glm::radians(-180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(110.f, 2.5f, -70.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));
    }


    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(-300.f, 0.f, 350.f));
    glm::mat4 offset = glm::mat4(1.f);
    offset = glm::translate(offset, glm::vec3(600.f, 300.f, -900.f));
    actors.push_back(world->createActor<engine::renderer::world::ACinematicCamera>(transform, offset, world));

    fireFinishedLoadingEvent();

    dynamic_cast<gamemode::PointGamemode*>(gamemode)->start();
}
