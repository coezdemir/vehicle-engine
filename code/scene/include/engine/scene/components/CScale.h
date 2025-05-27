//
// Created by Cem on 20.07.2022.
//
#pragma once
#include <engine/components/CComponent.h>
#include <engine/Engine_NS.h>
#include "CTransform.h"

namespace engine::scene::components
{
    /// \brief Applies scaling on a given transform every tick
    class CScale final : public engine::components::CComponent
    {
    public:
        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : engine::components::ConstructionInfo
        {
            bool bActiveFromStart = true;

            engine::scene::components::transform_handle_t transformComp;

            std::pair<glm::vec3, glm::vec3> scaleBetween;

            float speed;

            bool bShouldReverse;
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CScale(engine::components::id_t id = engine::components::invalid_id,
                engine::entities::id_t owner = engine::entities::invalid_id);

        /// \brief Initializes the component using physics::components::CRaycastVehicle::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* constructionInfo) override;

        void tick(float deltaTime) override;

        void activate();

        void deactivate();

        /// \brief Reset the scaling applied to the transform
        void reset();

    private:
        bool bIsActive;

        engine::scene::components::transform_handle_t transformComp;

        glm::mat4 initialTransform;

        std::pair<glm::vec3, glm::vec3> scaleBetween;

        float t;

        float speed;

        bool bShouldReverse;
    };

    using scale_container_t = typename engine::components::component_pool_t<CScale>;
    using scale_handle_t = typename scale_container_t::handle_t;
    using tscale_event_handle_t = typename scale_container_t::event_t::handle_t;
}