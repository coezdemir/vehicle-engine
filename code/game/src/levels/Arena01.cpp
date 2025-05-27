//
// Created by CemOe on 25.01.2022.
//

#include "Arena01.h"
#include <engine/renderer/world/ACinematicCamera.h>
#include "../world/level/AItem.h"
#include "../world/level/ARespawnPlane.h"

void game::level::Arena01::loadLevel(game::gamemode::Gamemode* gamemode)
{
    printf("-- Loading Arena01.\n");
    printf("\n *** HERE THE ACTORS ARE CREATED *** \n\n");

    fireStartedLoadingEvent();

    glm::mat4 transform = glm::mat4(1.f);

    std::vector<glm::mat4> playerTransforms;
    playerTransforms.push_back(glm::translate(transform, glm::vec3(-60.f, 0.5f, 40.f)));
    playerTransforms.push_back(glm::translate(transform, glm::vec3(-60.f, 0.5f, -40.f)));
    playerTransforms.push_back(glm::translate(transform, glm::vec3(60.f, 0.5f, 40.f)));
    playerTransforms.push_back(glm::translate(transform, glm::vec3(60.f, 0.5f, -40.f)));

    const std::vector<game::world::ACar*>& players= gamemode->getPlayers();
    for (int i = 0; i < players.size(); i++)
    {
        game::world::ACar* player = players[i];
        player->setStartTransform(playerTransforms[i]);
    }

    // Light
    transform = glm::mat4(1.f);
    lightActor = world->createActor<engine::renderer::world::ALight>(transform, world);
    actors.push_back(lightActor);

    // Respawn Plane
    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0.f, -100.f, 0.f));
    actors.push_back(world->createActor<game::world::ARespawnPlane>(transform, world));

    // Item Boxes
    {
        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(40.f, -3.f, 0.f));
        actors.push_back(world->createActor<game::world::AItem>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-40.f, -3.f, 0.f));
        actors.push_back(world->createActor<game::world::AItem>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-20.f, -3.f, -40.f));
        actors.push_back(world->createActor<game::world::AItem>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(20.f, -3.f, 40.f));
        actors.push_back(world->createActor<game::world::AItem>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(-20.f, -3.f, 40.f));
        actors.push_back(world->createActor<game::world::AItem>(transform, world));

        transform = glm::mat4(1.f);
        transform = glm::translate(transform, glm::vec3(20.f, -3.f, -40.f));
        actors.push_back(world->createActor<game::world::AItem>(transform, world));
    }

    transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.f));
    glm::mat4 offset = glm::mat4(1.f);
    offset = glm::translate(offset, glm::vec3(300.f, 300.f, -350.f));
    actors.push_back(world->createActor<engine::renderer::world::ACinematicCamera>(transform, offset, world));
}