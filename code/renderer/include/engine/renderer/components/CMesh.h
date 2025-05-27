//
// Created by Cem on 26.07.2022.
//
#pragma once
#include <engine/components/CComponent.h>
#include <engine/components/ComponentPool.h>
#include <engine/scene/ITraversableObject.h>
#include <engine/renderer/Mesh.h>
#include <engine/renderer/Shader.h>

namespace engine::renderer
{
    class RenderSystem;

    namespace components
    {
        /// \brief Holds a mesh and draws it.
        struct CMesh : public engine::components::CComponent, public engine::scene::ITraversableObject
        {
            friend RenderSystem;

        public:
            /// \brief Struct used in init(..) to initialize the component.
            struct ConstructionInfo : public engine::components::ConstructionInfo
            {
               std::vector<Vertex> verticesData;
               std::vector<unsigned int> indices;
               unsigned int triangleCount;
               Material material;
               std::vector<Texture> textures;
            };

            /// \brief Intended for internal usage only!
            /// \param id The id of the newly constructed component.
            /// \param owner The id of the entity that owns this component.
            explicit CMesh(engine::components::id_t id = engine::components::invalid_id,
                    entities::id_t owner = entities::invalid_id);

            /// \brief Initializes the component using physics::components::CMesh::ConstructionInfo.
            /// \param constructionInfo Info used to configure the component.
            void init(const engine::components::ConstructionInfo* constructionInfo) override;

            /// Sets if the mesh should be drawn or not.
            /// \param bIsVisible New visibility
            void setVisibility(bool bIsVisible);

            /// Override current mesh.
            /// \param newMes
            void setMesh(Model& newMesh);

            void initialVisit(scene::TreeTraverser* traverser) override;

            /// \brief Draws mesh upon visit.
            ///
            /// Call this by subscribing to the correct event in CTransform.
            /// \param treeTraverser Traverser traversing the scene graph.
            void visit(scene::TreeTraverser* traverser) override;

            [[nodiscard]] bool isVisible() const;

        protected:
            Mesh mesh;

            const renderer::Shader* shader{};

            bool bIsVisible;
        };

        using mesh_container_t = typename engine::components::component_pool_t<CMesh>;
        using mesh_handle_t = typename mesh_container_t::handle_t;
        using mesh_event_handle_t = typename mesh_container_t::event_t::handle_t;
    }
}