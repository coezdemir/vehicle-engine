//
// Created by CemOe on 06.12.2021.
//

#include "Racetrack03.h"
#include <engine/scene/components/CTranslation.h>
#include <engine/renderer/world/AEmissiveMesh.h>
#include <engine/renderer/world/ACinematicCamera.h>
#include "../world/level/ARespawnPlane.h"
#include "../world/level/AItem.h"
#include "../world/mesh/AMovingMesh.h"
#include "../world/obstacles/ARotatingBumper.h"
#include "../world/level/AGuide.h"
#include "../world/level/ABooster.h"

void game::level::Racetrack03::loadLevel(gamemode::Gamemode* gamemode)
{
    printf("\n -- Loading Racetrack03.\n");
    printf("\n *** HERE THE ACTORS ARE CREATED *** \n\n");

    fireStartedLoadingEvent();

    // Light
    glm::mat4 transform = glm::mat4(1.f);
    lightActor = world->createActor<engine::renderer::world::ALight>(transform, world);
    actors.push_back(lightActor);

    // Respawn Plane
    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(750.f, -5.f, -50.f));
    transform = glm::scale(transform, glm::vec3(2200.f, 10.f, 1600.f));
    actors.push_back(world->createActor<game::world::ARespawnPlane>(transform, world));

    // Guides
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-17.5f, 7.5f, 150.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(800.f, 7.5f, 82.5f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(732.5f, 7.5f, 450.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1100.f, 7.5f, 417.5f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1032.5f, 7.5f, 50.f));
        transform = glm::rotate(transform, glm::radians(-180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1600.f, 7.5f, 117.5f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1567.5f, 7.5f, -250.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(850.f, 7.5f, -182.5f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(917.5f, 7.5f, -550.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(550.f, 7.5f, -517.5f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(617.5f, 7.5f, -150.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(250.f, 7.5f, -182.5f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(317.5f, 7.5f, -600.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-50.f, 7.5f, -567.5f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::AGuide>(transform, world));
    }

    std::shared_ptr<game::world::ACheckpoint> lastCheckpoint;
    // Checkpoints
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(50.f, 0.f, 100.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(450.f, 0.f, 100.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1250.f, 0.f, 100.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1500.f, 0.f, -200.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1100.f, 0.f, -200.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(550.f, 0.f, -200.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        actors.push_back(world->createActor<world::ACheckpoint>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(0.f, 0.f, -500.f));
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        lastCheckpoint = world->createActor<world::ACheckpoint>(transform, world);
        actors.push_back(lastCheckpoint);
    }

    // Goal
    {
        transform = glm::mat4(1.f);
        transform = glm::scale(transform, glm::vec3(50.f, 10.f, 5.f));
        goalActor = world->createActor<world::AGoal>(transform, world, lastCheckpoint.get());
        goalActor->event_goalReached.subscribe([gamemode](game::world::ACar* car) {
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
            transform = glm::translate(transform, glm::vec3(150.f, 4.f, 100.f));
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
            transform = glm::translate(transform, glm::vec3(650.f, 4.f, 100.f));
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
            transform = glm::translate(transform, glm::vec3(1300.f, 4.f, 100.f));
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

        // Fourth item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(1000.f, 4.f, -200.f));
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

        // Fifth item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(450.f, 4.f, -200.f));
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

        // Sixth item box group
        {
            transform = glm::mat4(1.f);
            transform = glm::translate(transform, glm::vec3(150.f, 4.f, -550.f));
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

    // Hammer obstacles
    {
        transform = glm::mat4(1.f);
        glm::mat4 offset = glm::mat4(1.f);
        offset = glm::translate(offset, glm::vec3(5.f, 0.f, 0.f));
        engine::scene::components::CRotation::ConstructionInfo rotationInfo;
        rotationInfo.axis = glm::vec3(0.f, 1.f, 0.f);
        rotationInfo.stepSize = 1.f;

        transform = glm::translate(transform, glm::vec3(0.f, 3.f, -112.5f));
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(0.f, 0.f, -25.f));
        rotationInfo.stepSize = 1.1f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(62.5f, 0.f, 0.f));
        rotationInfo.stepSize = 1.15f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(-125.f, 0.f, 0.f));
        rotationInfo.stepSize = 1.2f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(62.5f, 0.f, -75.f));
        rotationInfo.stepSize = 1.25f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(37.5f, 0.f, 25.f));
        rotationInfo.stepSize = 1.3f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(-75.f, 0.f, 0.f));
        rotationInfo.stepSize = 1.35f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(37.5f, 0.f, -50.f));
        rotationInfo.stepSize = 1.4f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(62.5f, 0.f, 0.f));
        rotationInfo.stepSize = 1.5f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(-125.f, 0.f, 0.f));
        rotationInfo.stepSize = 1.55f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(62.5f, 0.f, -75.f));
        rotationInfo.stepSize = 1.6f;

        transform = glm::translate(transform, glm::vec3(-37.5f, 0.f, 25.f));
        rotationInfo.stepSize = 1.f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(75.f, 0.f, 0.f));
        rotationInfo.stepSize = 1.1f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(-37.5f, 0.f, -25.f));
        rotationInfo.stepSize = 1.15f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(62.5f, 0.f, 0.f));
        rotationInfo.stepSize = 1.2f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(-125.f, 0.f, 0.f));
        rotationInfo.stepSize = 1.25f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(62.5f, 0.f, -90.f));
        rotationInfo.stepSize = 1.3f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(37.5f, 0.f, 25.f));
        rotationInfo.stepSize = 1.35f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));

        transform = glm::translate(transform, glm::vec3(-75.f, 0.f, 0.f));
        rotationInfo.stepSize = 1.4f;
        actors.push_back(world->createActor<world::ARotatingBumper>(transform, offset, world, &rotationInfo));
    }

    // Wall obstacles
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(362.5f, 30.f, 100.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        engine::scene::components::CTranslation::ConstructionInfo translationInfo;
        translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) - glm::vec3(0.f, 50.f, 0.f)};
        translationInfo.speed = 10.f;
        translationInfo.bShouldReverse = true;
        translationInfo.cooldownTimeSec = 3.f;
        actors.push_back(
                world->createActor<world::AMovingMesh>(transform, world, "game/obstacles/wall/SmallWall.obj",translationInfo));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1187.5f, 30.f, -200.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        translationInfo.pointsToMoveBetween = {glm::vec3(transform[3]), glm::vec3(transform[3]) - glm::vec3(0.f, 50.f, 0.f)};
        translationInfo.speed = 12.f;
        actors.push_back(
                world->createActor<world::AMovingMesh>(transform, world, "game/obstacles/wall/SmallWall.obj", translationInfo));
    }

    // Booster
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(730.f, 2.5f, 190.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(860.f, 2.5f, 380.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1070.f, 2.5f, 335.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1125.f, 2.5f, 80.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1420.f, 2.5f, 120.f));
        transform = glm::rotate(transform, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(1530.f, 2.5f, -6.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(920.f, 2.5f, -260.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(825.f, 2.5f, -480.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(660.f, 2.5f, -520.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(620.f, 2.5f, -430.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(540.f, 2.5f, -220.f));
        transform = glm::rotate(transform, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(580.f, 2.5f, -290.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(280.f, 2.5f, -280.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(320.f, 2.5f, -440.f));
        transform = glm::rotate(transform, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        actors.push_back(world->createActor<world::ABooster>(transform, world));
    }

    fireFinishedLoadingEvent();

    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(625.f, 0.f, -100.f));
    glm::mat4 offset = glm::mat4(1.f);
    offset = glm::translate(offset, glm::vec3(0.f, 650.f, -1200.f));
    actors.push_back(world->createActor<engine::renderer::world::ACinematicCamera>(transform, offset, world));

    dynamic_cast<gamemode::PointGamemode*>(gamemode)->restart(600);
    //dynamic_cast<gamemode::PointGamemode*>(gamemode)->start();
}
