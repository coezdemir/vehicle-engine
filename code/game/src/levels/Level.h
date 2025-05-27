//
// Created by CemOe on 21.11.2021.
//
#pragma once
#include <engine/world/World.h>
#include <engine/renderer/Mesh.h>
#include <engine/renderer/components/CModel.h>
#include "engine/physics/components/collision/CComplexCollision.h"
#include <engine/renderer/world/ALight.h>
#include <engine/renderer/world/ACamera.h>
#include "../world/AMyActor.h"
#include "../world/ACar.h"
#include "../world/level/AGoal.h"
#include "../gamemode/Gamemode.h"

namespace game::level
{
    class ALevel : public world::AMyActor
    {
    public:
        /* Events */
        using event_t = typename engine::callbacks::Event<ALevel>;

        explicit ALevel(engine::world::World* world, std::string_view filename, int id)
            : AMyActor(glm::mat4(1.f), world), world(world), id(id)
        {
            // Init mesh component
            meshComp = rootEntity.addComponent<engine::renderer::components::CModel>();
            engine::renderer::components::CModel::ConstructionInfo meshInfo;
            meshInfo.instantiator = this;
            meshInfo.path = filename;
            meshComp->init(&meshInfo);

            // Init mesh collision
            complexCollisionComp = rootEntity.addComponent<engine::physics::components::CComplexCollision>();
            engine::physics::components::CComplexCollision::ConstructionInfo collisionInfo;
            collisionInfo.instantiator = this;
            collisionInfo.bIsDynamic = false;
            collisionInfo.bIsSimulationShape = true;
            collisionInfo.bShouldDeactivateAfterCollision = false;
            collisionInfo.timeActive = -1.f;
            collisionInfo.bUpdateLocation = false;
            meshComp->getAllVertices(vertices);
            meshComp->getAllIndices(indices);
            collisionInfo.vertices = &vertices;
            collisionInfo.indices = &indices;
            collisionInfo.bActiveFromStart = true;
            complexCollisionComp->init(&collisionInfo);

            transformComp->event_visited.subscribe([this](engine::scene::TreeTraverser* traverser){
                meshComp->visit(traverser);
                complexCollisionComp->visit(traverser);
            });

            actors.reserve(100);
        }

        explicit ALevel(engine::world::World* world)
                : AMyActor(glm::mat4(1.f), world), world(world)
        {
            actors.reserve(100);
        }

        virtual void loadLevel(gamemode::Gamemode* gamemode) = 0;

        virtual void unloadLevel()
        {
            for (int i = 0; i < actors.size(); ++i)
                actors[i]->destroy();

            actors.clear();
        }

        int getId() const
        {
            return id;
        }

    protected:
        virtual void fireStartedLoadingEvent()
        {
            event_startedLoading.fire();
        }

        virtual void fireFinishedLoadingEvent()
        {
            event_finishedLoading.fire();
        }

    public:
        event_t event_startedLoading;

        event_t event_finishedLoading;

    protected:
        int id;

        engine::world::World* world;

        engine::renderer::components::model_handle_t meshComp;

        engine::physics::components::complexCollision_handle_t complexCollisionComp;

        std::vector<std::shared_ptr<AActor>> actors;

        std::shared_ptr<game::world::ACar> player1;

        std::shared_ptr<game::world::ACar> player2;

        std::shared_ptr<engine::renderer::world::ALight> lightActor;

        std::shared_ptr<world::AGoal> goalActor;

        world::AGoal::goalReached_event_t::handle_t handle_goalReached;

        std::vector<std::vector<glm::vec3>> vertices;

        std::vector<std::vector<unsigned int>> indices;

        std::vector<unsigned int> triangleCount;
    };
}
