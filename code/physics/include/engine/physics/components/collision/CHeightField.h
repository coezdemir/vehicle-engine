//
// Created by Cem on 26.06.2022.
//
#pragma once
#include "CCollision.h"

namespace engine::physics::components
{
    struct CHeightField final
            : public engine::physics::components::CCollision
    {
        friend PhysicsSystem;

    public:
        struct ConstructionInfo : public engine::physics::components::CCollision::ConstructionInfo
        {
            physx::PxHeightFieldDesc hfDesc;
            float rowScale;
            float colScale;
            float heightScale;
        };

        explicit CHeightField(engine::components::id_t id = engine::components::invalid_id,
                              entities::id_t owner = entities::invalid_id);

        CHeightField& operator=(CHeightField&& other) noexcept
        {
            if (this != &other)
            {
                CPhysicsComponent::operator=(std::move(other));
                pxCooking = other.pxCooking;
                hfDesc = other.hfDesc;
                heightField = other.heightField;
                rowScale = other.rowScale;
                colScale = other.colScale;
                heightScale = other.heightScale;

                return *this;
            }
            return *this;
        }

        void init(const engine::components::ConstructionInfo* constructionInfo) override;

        void initPhysics(const glm::mat4& initTransform) override;

        void destroyPhysics() override;

    private:
        physx::PxCooking* pxCooking;

        physx::PxHeightFieldDesc hfDesc;

        physx::PxHeightField* heightField;

        float rowScale;

        float colScale;

        float heightScale;
    };

    using heightField_container_t = typename engine::components::component_pool_t<CHeightField>;
    using heightField_handle_t = typename heightField_container_t::handle_t;
    using heightField_container_event_handle = typename heightField_container_t::event_t::handle_t;
}


