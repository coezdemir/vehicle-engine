//
// Created by CemOe on 15.08.2021.
//
#pragma once
#include <engine/components/CComponent.h>
#include <engine/scene/ITraversableObject.h>
#include <glm/glm.hpp>
#include <engine/components/ComponentPool.h>
#include <engine/callbacks/ComponentEvent.h>

namespace engine::scene
{
    class SceneGraph;

    class SceneSystem;

    class TreeTraverser;

    namespace components
    {
        /// \brief Component that holds transform information
        struct CTransform final : public engine::components::CComponent
        {
        public:
            /* Events */
            using attachment_event_t = typename engine::callbacks::ComponentEvent<CTransform, engine::components::id_t>;
            using visit_event_t = typename engine::callbacks::ComponentEvent<CTransform, TreeTraverser*>;

            /// \brief Struct used in init(..) to initialize the component.
            struct ConstructionInfo : engine::components::ConstructionInfo
            {
                glm::mat4 transform;
            };

            /// \brief Intended for internal usage only!
            /// \param id The id of the newly constructed component.
            /// \param owner The id of the entity that owns this component.
            explicit CTransform(engine::components::id_t id = engine::components::invalid_id,
                                entities::id_t owner = entities::invalid_id);

            ~CTransform() override
            {
                children.clear();
                event_transformCompAttached.unsubscribe();
                event_init.unsubscribe();
                event_visited.unsubscribe();
            }

            CTransform& operator=(CTransform&& other) noexcept
            {
                CComponent::operator=(other);

                if (this != &other)
                {
                    transform = other.transform;
                    localTransform = other.localTransform;
                    children = other.children;
                    event_transformCompAttached = other.event_transformCompAttached;
                    event_visited = other.event_visited;
                    event_init = other.event_init;

                    other.children.clear();
                    other.event_transformCompAttached.unsubscribe();
                    other.event_visited.unsubscribe();
                    other.event_init.unsubscribe();
                    return *this;
                }

                return *this;
            }

            /// \brief Initializes the component using physics::components::CTransform::ConstructionInfo.
            /// \param constructionInfo Info used to configure the component.
            void init(const engine::components::ConstructionInfo* constructionInfo) override;

            /// \brief Initialize transform data and calls event_init.
            /// Only gets called at the beginning of runtime.
            /// \param treeTraverser Traverser traversing the scene graph.
            void initialVisit(TreeTraverser* traverser);

            /// \brief Update transform data and calls event_visit.
            /// \param treeTraverser Traverser traversing the scene graph.
            void visit(TreeTraverser* traverser);

            /// \brief Update transform data.
            /// Only gets called at the beginning of runtime
            /// \param treeTraverser Traverser traversing the scene graph.
            void visitWithoutEvent(TreeTraverser* traverser);

            /// \brief Add a CTransform as a child.
            /// \param newChild CTransform to attach to this component.
            void addChild(engine::components::id_t newChild);

            /// \brief Removes a CTransform as a child.
            /// \param newChild CTransform to detach to this component.
            void removeChild(engine::components::id_t childToRemove);

            /// \brief Apply translation to the transform
            /// \param translation Translation to apply
            void translate(glm::vec3 translation);

            /// \brief Apply rotation to the transform
            /// \param translation Rotation to apply
            void rotate(float angleInDegree, glm::vec3 axis);

            /// \brief Set transform in world space
            /// \param newTransform
            void setTransform(glm::mat4 newTransform);

            /// \brief Set transform in local space
            /// \param localTransform
            void setLocalTransform(glm::mat4 localTransform);

            [[nodiscard]] const std::vector<engine::components::id_t>& getChildren() const;

            [[nodiscard]] glm::mat4 getLocalTransform();

            [[nodiscard]] glm::mat4 getTransform();

            [[nodiscard]] glm::vec3 getForward() const;

            [[nodiscard]] const engine::components::id_t& getParent() const;

            attachment_event_t event_transformCompAttached;

            visit_event_t event_init;

            visit_event_t event_visited;

        private:
            glm::mat4 transform;

            glm::mat4 localTransform;

            std::vector<engine::components::id_t> children;
        };

        using transform_container_t = typename engine::components::component_pool_t<CTransform>;
        using transform_handle_t = typename transform_container_t::handle_t;
        using transform_event_handle = typename transform_container_t::event_t::handle_t;
    }
}