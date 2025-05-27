//
// Created by CemOe on 06.11.2021.
//
#pragma once
#include <engine/components/CComponent.h>
#include <glm/glm.hpp>
#include <engine/Engine_NS.h>
#include "CTransform.h"

namespace engine::scene::components
{
    /// \brief Applies a translation on a given transform every tick
    class CTranslation final : public engine::components::CComponent
    {
    public:
        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : engine::components::ConstructionInfo
        {
            engine::scene::components::transform_handle_t transformComp;

            std::pair<glm::vec3, glm::vec3> pointsToMoveBetween;

            float speed;

            bool bShouldReverse;

            float cooldownTimeSec;
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CTranslation(engine::components::id_t id = engine::components::invalid_id,
                              engine::entities::id_t owner = engine::entities::invalid_id);

        /// \brief Initializes the component using physics::components::CRaycastVehicle::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* constructionInfo) override;

        void tick(float deltaTime) override;

    private:
        void reverse();

        void checkForCooldown(float deltaTime);

        engine::scene::components::transform_handle_t transformComp;

        std::pair<glm::vec3, glm::vec3> pointsToMoveBetween;

        float t;

        float currentDeltaTime;

        float speed;

        bool bShouldReverse;

        bool bIsCoolingDown;

        float cooldownTimeSec;

        float cooldownSteps;
    };

    using translation_container_t = typename engine::components::component_pool_t<CTranslation>;
    using translation_handle_t = typename translation_container_t::handle_t;
    using translation_event_handle_t = typename translation_container_t::event_t::handle_t;
}

