//
// Created by CemOe on 29.12.2020.
//
#pragma once
#include <iostream>
#include <engine/physics/components/collision/CSphereCollision.h>
#include <engine/audio/Audio.h>
#include <engine/audio/components/CAudio.h>
#include "../mesh/AStaticMesh.h"
#include "../ACar.h"
#include "../AMyActor.h"

namespace game::world
{
    class AItem final : public AMyActor
    {
    public:
        AItem(glm::mat4 transform, engine::world::World* world);

        void itemCollected(ACar* car);

        void tick(float deltaTime) override;

    private:
        engine::renderer::components::model_handle_t meshComp;

        engine::physics::components::sphereCollision_handle_t sphereCollisionComp;

        engine::audio::components::audio_handle_t collectedSound;

        bool bWasCollected;

        bool bIsRespawning;

        int respawnSteps;

        const float RESPAWN_TIME_SEC = 10.f;
    };
}
