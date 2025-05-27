//
// Created by CemOe on 27.08.2021.
//
#pragma once
#include <engine/physics/components/collision/CCollision.h>
#include <engine/world/AActor.h>

namespace engine::physics::components
{
    /// \brief Creates and manages a sphere collision.
    struct CSphereCollision : public CCollision
    {
        friend PhysicsSystem;

    public:
        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : public engine::physics::components::CCollision::ConstructionInfo
        {
            float sphereRadius;
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CSphereCollision(engine::components::id_t id = engine::components::invalid_id,
                                  entities::id_t owner = entities::invalid_id);

        CSphereCollision& operator=(CSphereCollision&& other) noexcept
        {
            if(this != &other)
            {
                CCollision::operator=(std::move(other));
                sphereRadius = other.sphereRadius;
                return *this;
            }
            return *this;
        }

        /// \brief Initializes the component using physics::components::CSphereCollision::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* constructionInfo) override;

        /// \brief Initialize physics.
        ///
        /// Creates the sphere rigid body or the sphere trigger shape based in configuration.
        /// \param initTransform Initial transform.
        void initPhysics(const glm::mat4& transform) override;

    private:
        float sphereRadius;
    };

    using sphereCollision_container_t = typename engine::components::component_pool_t<CSphereCollision>;
    using sphereCollision_handle_t = typename sphereCollision_container_t::handle_t;
    using sphereCollision_event_handle = typename sphereCollision_container_t::event_t::handle_t;
}