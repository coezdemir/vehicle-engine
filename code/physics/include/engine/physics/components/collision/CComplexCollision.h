//
// Created by CemOe on 24.08.2021.
//
#pragma once
#include <engine/scene/ITraversableObject.h>
#include <engine/callbacks/Event.h>
#include <engine/physics/components/collision/CCollision.h>

namespace engine::physics::components
{
    /// \brief Creates and manages a complex collision.
    struct CComplexCollision final
            : public engine::physics::components::CCollision
    {
        friend PhysicsSystem;

    public:
        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : public engine::physics::components::CCollision::ConstructionInfo
        {
        public:
            std::vector<std::vector<glm::vec3>>* vertices;
            std::vector<std::vector<unsigned int>>* indices;
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CComplexCollision(engine::components::id_t id = engine::components::invalid_id,
                                   entities::id_t owner = entities::invalid_id);

        CComplexCollision& operator=(CComplexCollision&& other) noexcept
        {
            if(this != &other)
            {
                CCollision::operator=(std::move(other));
                pxCooking = other.pxCooking;
                vertices = other.vertices;
                indices = other.indices;
                triangleMesh = other.triangleMesh;

                other.vertices = nullptr;
                other.indices = nullptr;
                other.triangleMesh = nullptr;

                return *this;
            }
            return *this;
        }

        /// \brief Initializes the component using physics::components::CComplexCollision::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* constructionInfoPtr) override;

        /// \brief Initialize physics.
        ///
        /// Creates the complex rigid body.
        /// \param initTransform Initial transform.
        void initPhysics(const glm::mat4& transform) override;

    private:
        physx::PxCooking* pxCooking;

        std::vector<std::vector<glm::vec3>>* vertices;

        std::vector<std::vector<unsigned int>>* indices;

        physx::PxTriangleMesh* triangleMesh;
    };

    using complexCollision_container_t = typename engine::components::component_pool_t<CComplexCollision>;
    using complexCollision_handle_t = typename complexCollision_container_t::handle_t;
    using complexCollision_event_handle = typename complexCollision_container_t::event_t::handle_t;
}
