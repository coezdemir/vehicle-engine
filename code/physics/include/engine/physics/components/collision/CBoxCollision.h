//
// Created by CemOe on 27.08.2021.
//
#pragma once
#include "CCollision.h"

namespace engine::physics::components
{
    /// \brief Creates and manages a box collision.
    struct CBoxCollision
            : public CCollision
    {
        friend PhysicsSystem;

    public:
        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : public engine::physics::components::CCollision::ConstructionInfo
        {
            glm::vec3 boxHalfExtent;
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CBoxCollision(engine::components::id_t id = engine::components::invalid_id,
                               entities::id_t owner = entities::invalid_id);

        CBoxCollision& operator=(CBoxCollision&& other) noexcept
        {
            if (this != &other)
            {
                CCollision::operator=(std::move(other));
                boxHalfExtent = other.boxHalfExtent;

                return *this;
            }
            return *this;
        }

        /// \brief Initializes the component using physics::components::CBoxCollision::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* constructionInfo) override;

        /// \brief Initialize physics.
        ///
        /// Creates the box rigid body or the box trigger shape based in configuration.
        /// \param initTransform Initial transform.
        void initPhysics(const glm::mat4& transform) override;

    private:
        glm::vec3 boxHalfExtent;
    };

    using boxCollision_container_t = typename engine::components::component_pool_t<CBoxCollision>;
    using boxCollision_handle_t = typename boxCollision_container_t::handle_t;
    using boxCollision_event_handle = typename boxCollision_container_t::event_t::handle_t;
}