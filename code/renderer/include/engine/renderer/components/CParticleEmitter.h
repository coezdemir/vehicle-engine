//
// Created by Cem on 14.07.2022.
//
#pragma once
#include <engine/components/CComponent.h>
#include <engine/renderer/Shader.h>
#include <engine/scene/ITraversableObject.h>
#include <engine/renderer/particle/ParticleSystem.h>
#include <engine/components/ComponentPool.h>

namespace engine::renderer
{
    class RenderSystem;

    namespace components
    {
        /// \brief Holds a particle system and draws it.
        struct CParticleEmitter final : public engine::components::CComponent, public engine::scene::ITraversableObject
        {
            friend RenderSystem;

        public:
            /// \brief Struct used in init(..) to initialize the component.
            struct ConstructionInfo : public engine::components::ConstructionInfo
            {
                renderer::particle::ParticleSystemProps* particleSystemProps;
            };

            /// \brief Intended for internal usage only!
            /// \param id The id of the newly constructed component.
            /// \param owner The id of the entity that owns this component.
            explicit CParticleEmitter(engine::components::id_t id = engine::components::invalid_id,
                                      entities::id_t owner = entities::invalid_id);

            /// \brief Initializes the component using physics::components::CMesh::ConstructionInfo.
            /// \param constructionInfo Info used to configure the component.
            void init(const engine::components::ConstructionInfo* constructionInfo) override;

            void tick(float deltaTime) override;

            void initialVisit(scene::TreeTraverser* traverser) override;

            /// \brief Draws the particles upon visit.
            ///
            /// Call this by subscribing to the correct event in CTransform.
            /// \param treeTraverser Traverser traversing the scene graph.
            void visit(scene::TreeTraverser* traverser) override;

            /// \brief Activate particle system
            void activate();

            /// \brief Deactivate particle system
            void deactivate();

        private:
            renderer::particle::ParticleSystem particleSystem;

            const renderer::Shader* shader;

            const renderer::Shader* meshShader;
        };

        using particle_container_t = typename engine::components::component_pool_t<CParticleEmitter>;
        using particle_handle_t = typename particle_container_t::handle_t;
        using particleContainer_event_handle_t = typename particle_container_t::event_t::handle_t;
    }
}