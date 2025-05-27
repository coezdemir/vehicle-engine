//
// Created by CemOe on 23.08.2021.
//
#pragma once
#include <engine/components/CComponent.h>
#include <engine/scene/ITraversableObject.h>
#include <engine/components/ComponentPool.h>
#include "../Shader.h"

namespace engine::renderer::components
{
    /// \brief Represents a light.
    struct CLight final : public engine::components::CComponent, public engine::scene::ITraversableObject
    {
        friend class RenderSystem;

    public:
        /// \brief Struct used in init(..) to initialize the component.
        struct ConstructionInfo : public engine::components::ConstructionInfo
        {
            glm::vec4 vector;
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;

            ConstructionInfo()
            {
                vector = glm::vec4(0.2f, 1.f, 0.3f, 0);
                ambient = glm::vec3(0.4f);
                diffuse = glm::vec3(0.5f);
                specular = glm::vec3(0.2f);
            }

            ConstructionInfo(glm::vec4 vector, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
                :vector(vector), ambient(ambient), diffuse(diffuse), specular(specular)
            {
            }
        };

        /// \brief Intended for internal usage only!
        /// \param id The id of the newly constructed component.
        /// \param owner The id of the entity that owns this component.
        explicit CLight(engine::components::id_t id = engine::components::invalid_id,
                        entities::id_t owner = entities::invalid_id);

        /// \brief Initializes the component using physics::components::CLight::ConstructionInfo.
        /// \param constructionInfo Info used to configure the component.
        void init(const engine::components::ConstructionInfo* constructionInfo) override;

        /// \brief Returns light properties.
        /// \return Light properties.
        [[nodiscard]] ConstructionInfo getLightProperties() const;

        /// \brief Sets up light properties in the shader
        ///
        /// Call this by subscribing to the correct event in CTransform.
        /// \param treeTraverser Traverser traversing the scene graph.
        void initialVisit(scene::TreeTraverser* traverser) override;

        /// \brief Can be overridden to create various light effects.
        ///
        /// Is currently just a stub that updates model matrix.
        /// \param treeTraverser Traverser traversing the scene graph.
        void visit(scene::TreeTraverser* traverser) override;

    private:
        glm::vec4 vector;

        glm::vec3 ambient;

        glm::vec3 diffuse;

        glm::vec3 specular;

        const renderer::Shader* shader;
    };

    using light_container_t = typename engine::components::component_pool_t<CLight>;
    using light_handle_t = typename light_container_t::handle_t;
    using light_event_handle = typename light_container_t::event_t::handle_t;
}
