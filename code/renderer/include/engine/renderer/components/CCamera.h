//
// Created by CemOe on 19.09.2021.
//
#pragma once
#include <engine/components/CComponent.h>
#include <engine/scene/ITraversableObject.h>
#include <engine/renderer/Shader.h>
#include <engine/components/ComponentPool.h>

namespace engine::renderer
{
    class RenderSystem;

    namespace components
    {
        enum ECameraType
        {
            CT_PERSPECTIVE = 0,
            CT_ORTHOGRAPHIC = 1
        };

        /// \brief Represents a camera.
        struct CCamera final
                : public engine::components::CComponent, public engine::scene::ITraversableObject
        {
            friend RenderSystem;

        public:
            /// \brief Struct used in init(..) to initialize the component.
            struct ConstructionInfo : public engine::components::ConstructionInfo
            {
                ECameraType cameraType;
                std::function<glm::vec3()> targetLoc;
                glm::vec4 up;
                float fovy;
                float left;
                float right;
                float bottom;
                float top;
                float zNear;
                float zFar;
                bool bIsCinematic;

                ConstructionInfo()
                {
                    cameraType = CT_PERSPECTIVE;
                    targetLoc = ([](){return glm::vec3(0.f);});
                    up = glm::vec4(0.f, 1.f, 0.f, 0.f);
                    fovy = 40.f;
                    left = -1000.f;
                    right = 1000.f;
                    bottom = -800.f;
                    top = 800.f;
                    zNear = 0.1f;
                    zFar = 10000.f;
                    bIsCinematic = false;
                }
            };

            /// \brief Intended for internal usage only!
            /// \param id The id of the newly constructed component.
            /// \param owner The id of the entity that owns this component.
            explicit CCamera(engine::components::id_t id = engine::components::invalid_id,
                             entities::id_t owner = entities::invalid_id);

            /// \brief Initializes the component using physics::components::CCamera::ConstructionInfo.
            /// \param constructionInfo Info used to configure the component.
            void init(const engine::components::ConstructionInfo* constructionInfo) override;

            /// \brief Returns the location the camera is looking at in world transform.
            /// \return Location as a glm::vec3
            [[nodiscard]] glm::vec3 getTargetActorLoc() const;

            /// \brief Returns up direction
            /// \return Direction vector
            [[nodiscard]] const glm::vec4& getUp() const;

            [[nodiscard]] float getFovy() const;

            [[nodiscard]] float getZNear() const;

            [[nodiscard]] float getZFar() const;

            /// \brief Registers the camera to the scene graph.
            ///
            /// Call this by subscribing to the correct event in CTransform.
            /// \param treeTraverser Traverser traversing the scene graph.
            void initialVisit(scene::TreeTraverser* traverser) override;

            /// \brief Used to update various transforms based on configuration.
            ///
            /// Updates matrices such as projection and view.
            /// \param treeTraverser Traverser traversing the scene graph.
            void visit(scene::TreeTraverser* traverser) override;

        private:
            /// Calculates aspect ratio.
            /// \param numberOfViewports Number of viewports.
            /// \return Aspect ratio.
            float calculateAspect(int numberOfViewports);

            ECameraType cameraType;

            std::function<glm::vec3()> targetLoc;

            glm::vec4 up;

            float fovy;

            float left;

            float right;

            float bottom;

            float top;

            float zNear;

            float zFar;

            int cameraID;

            bool bIsCinematic;

            float windowWidth;

            float windowHeight;

            const renderer::Shader* shader = nullptr;
        };

        using cameraComponent_container_t = typename engine::components::component_pool_t<CCamera>;
        using cameraComponent_handle_t = typename cameraComponent_container_t::handle_t;
        using cameraComponent_event_handle = typename cameraComponent_container_t::event_t::handle_t;
    }

}
