//
// Created by CemOe on 08.12.2021.
//
#pragma once
#include <engine/components/CComponent.h>
#include <engine/Engine_NS.h>
#include <glm/glm.hpp>
#include "CTransform.h"

namespace engine::scene::components
{
    /// \brief Applies a rotation until it reaches a certain value, sleeps for a configured time and resumes the rotation
    class CStepRotation final : public engine::components::CComponent
    {
    public:
        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : engine::components::ConstructionInfo
        {
            engine::scene::components::transform_handle_t transformComp;

            glm::vec3 axis;

            float goalValue;

            float speed;

            bool bShouldReverse;

            float cooldownTimeSec;
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CStepRotation(engine::components::id_t id = engine::components::invalid_id,
                               engine::entities::id_t owner = engine::entities::invalid_id);

        /// \brief Initializes the component using physics::components::CRaycastVehicle::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* constructionInfo) override;

        void tick(float deltaTime) override;

    private:
        engine::scene::components::transform_handle_t transformComp;

        glm::vec3 axis;

        float goalValue;

        float currentGoalValue;

        float speed;

        bool bShouldReverse;

        float direction;

        bool bShouldCooldown;

        float cooldownTimeSec;

        int cooldownSteps;

        bool bIsCoolingDown;
    };

    using stepRotation_container_t = typename engine::components::component_pool_t<CStepRotation>;
    using stepRotation_handle_t = typename stepRotation_container_t::handle_t;
    using stepRotation_event_handle_t = typename stepRotation_container_t::event_t::handle_t;
}
