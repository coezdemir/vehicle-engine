//
// Created by Cem on 21.07.2022.
//
#pragma once
#include <engine/physics/components/collision/CBoxCollision.h>
#include <engine/renderer/components/CModel.h>
#include "../AMyActor.h"

namespace game::world
{
    class ABooster final : public AMyActor
    {
    public:
        ABooster(glm::mat4 transform, engine::world::World* world);

    private:
        static void applyBoost(const physx::PxRigidActor* body);

        engine::scene::components::transform_handle_t collisionTransformComp;

        engine::physics::components::boxCollision_handle_t boxCollisionComp;

        engine::renderer::components::model_handle_t meshComp;

        static constexpr float BOOST_VALUE = 20000.f;
    };
}