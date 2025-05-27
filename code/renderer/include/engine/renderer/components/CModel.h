//
// Created by CemOe on 15.08.2021.
//
#pragma once
#include <engine/components/CComponent.h>
#include <engine/renderer/Mesh.h>
#include <engine/renderer/Shader.h>
#include <engine/components/ComponentPool.h>
#include <engine/scene/ITraversableObject.h>
#include "engine/renderer/manager/ModelManager.h"

namespace engine::renderer
{
    class RenderSystem;

    namespace components
    {
        /// \brief Holds a model and draws it.
        struct CModel final : public engine::components::CComponent, public engine::scene::ITraversableObject
        {
            friend RenderSystem;

        public:
            /// \brief Struct used in init(..) to initialize the component.
            struct ConstructionInfo : public engine::components::ConstructionInfo
            {
                std::string_view path;
                bool bFlunctuateEmission = false;
                float flunctuationInterval = 1.f;
                float flunctuationDelay = 0.f;
            };

            /// \brief Intended for internal usage only!
            /// \param id The id of the newly constructed component.
            /// \param owner The id of the entity that owns this component.
            explicit CModel(engine::components::id_t id = engine::components::invalid_id,
                           entities::id_t owner = entities::invalid_id);

            ~CModel() override;

            CModel& operator=(CModel&& other) noexcept
            {
                CComponent::operator=(other);

                if (this != &other)
                {
                    manager::ModelManager::lostUser(mesh);
                    mesh = other.mesh;
                    manager::ModelManager::addUser(other.mesh);
                    shader = other.shader;
                    bIsVisible = other.bIsVisible;
                    bFlunctuateEmission = other.bFlunctuateEmission;
                    flunctuationInterval = other.flunctuationInterval;
                    flunctuationDelay = other.flunctuationDelay;

                    manager::ModelManager::lostUser(other.mesh);
                    other.mesh = manager::ModelHandle();

                    return *this;
                }

                return *this;
            }

            /// \brief Initializes the component using physics::components::CMesh::ConstructionInfo.
            /// \param constructionInfo Info used to configure the component.
            void init(const engine::components::ConstructionInfo* constructionInfo) override;

            /// \brief Change the shader that is used to draw the mesh.
            /// \param newShader New shader to use.
            void changeShader(const renderer::Shader* newShader);

            /// Sets if the mesh should be drawn or not.
            /// \param bIsVisible New visibility
            void setVisibility(bool bIsVisible);

            /// Override current mesh.
            /// \param newMeshPath Path to the new mesh.
            void setModel(std::string_view newMeshPath);

            void initialVisit(scene::TreeTraverser* traverser) override;

            /// \brief Draws mesh upon visit.
            ///
            /// Call this by subscribing to the correct event in CTransform.
            /// \param treeTraverser Traverser traversing the scene graph.
            void visit(scene::TreeTraverser* traverser) override;

            [[nodiscard]] bool isVisible() const;

            void getAllVertices(std::vector<std::vector<glm::vec3>>& allVertices);

            void getAllIndices(std::vector<std::vector<unsigned int>>& allIndices);

            void getAllTriangleCounts(std::vector<unsigned int>& allTriangleCounts);

        private:
            renderer::manager::ModelHandle mesh;

            const renderer::Shader* shader;

            bool bIsVisible;

            bool bFlunctuateEmission;

            float flunctuationInterval;

            float flunctuationDelay;
        };

        using model_container_t = typename engine::components::component_pool_t<CModel>;
        using model_handle_t = typename model_container_t::handle_t;
        using model_event_handle_t = typename model_container_t::event_t::handle_t;
    }
}