//
// Created by coezdemir on 21.11.2020.
//
#pragma once
#include <engine/renderer/components/CCamera.h>
#include <engine/audio/components/CAudio.h>
#include <engine/physics/components/collision/CSphereCollision.h>
#include <engine/physics/world/ARaycastVehicle.h>
#include <engine/renderer/components/CParticleEmitter.h>
#include <engine/renderer/manager/ModelManager.h>
#include <engine/scene/components/CScale.h>
#include "level/ACheckpoint.h"
#include "../enums/ECarState.h"
#include "../enums/EItem.h"
#include "../gamemode/PointGamemode.h"

namespace game
{
    class Game;

    namespace world
    {
        class ARespawnPlane;
        class AItem;
        class ACamera;

        class ACar : public engine::physics::world::ARaycastVehicle
        {
            friend game::Game;
            friend game::gamemode::PointGamemode;

        public:
            /* Events */
            using respawn_event_t = typename engine::callbacks::Event<ACar, ACar*>;

            ACar(glm::mat4 transform, engine::world::World* world, const engine::physics::PhysicsSystem* physicsSystem);

            void tick(float deltaTime) override;

            void respawn(bool bDueToFlip = false);

            [[nodiscard]] std::function<void()> reset();

            void setCurrentItem(EItem newItem);

            void setCheckpoint(const game::world::ACheckpoint* checkpoint);

            void setStartTransform(const glm::mat4& startTransform);

            void setCollided(bool value, const physx::PxVec3& forceVec, ACar* otherCar);

            void setID(int newID);

            bool collided() const;

            [[nodiscard]] int getId() const;

            [[nodiscard]] ECarState getState() const;

            respawn_event_t event_respawned;

            [[nodiscard]] EItem getCurrentItem() const;

            [[nodiscard]] const game::world::ACheckpoint* getLastCheckpoint() const;
        private:
            void activateItem();

            void applyItemEffect(EItem item);

            void activateEngineOffItem();

            void keyInput(int key, int scanecode, int action, int mods);

            void controllerInput(int controllerID, GLFWgamepadstate controllerState);

            bool checkIfFlippedOver();

            void playCrashSound();

            void handleEngineOffCollision(const engine::physics::components::CollisionInfo& collisionInfo);

            void setLastCollidedCar(ACar* lastCollidedCar);

            void resetItemState();

        private:
            const float ENGINE_OFF_TRIGGER_ACTIVE_TIME = 5.f;

            const float RESET_TIME_LAST_COLLIDED_CAR = 30.f;

            int id;

            engine::renderer::components::model_handle_t meshComp;

            engine::scene::components::transform_handle_t camTransformComp;

            engine::renderer::components::cameraComponent_handle_t cameraComp;

            engine::physics::components::sphereCollision_handle_t sphereCollisionComp;

            engine::scene::components::transform_handle_t particleTransformComp;

            engine::renderer::components::particle_handle_t knockbackDownParticle;

            engine::renderer::components::particle_handle_t knockbackUpParticle;

            engine::scene::components::transform_handle_t manipulationTransformComp;

            engine::scene::components::scale_handle_t scaleComp;

            engine::renderer::components::model_handle_t meshManipulation;

            engine::renderer::components::model_handle_t ghostMesh;

            ECarState state;

            EItem currentItem;

            float itemActiveTime;

            const game::world::ACheckpoint* lastCheckpoint;

            glm::mat4 startTransform;

            ACar* lastCollidedCar;

            float currentRestTimeCollidedCar;

            engine::audio::components::audio_handle_t crashSound;

            bool bCollided;

            physx::PxVec3 force;

            physx::PxScene* pxScene;

            bool bFlippedOver;
        };
    }
}



