//
// Created by coezdemir on 18.11.2020.
//
#pragma once
#include <engine/entities/Entity.h>
#include <glm/glm.hpp>
#include <vector>
#include <stack>
#include "SceneGraph.h"
#include "components/CTransform.h"

namespace engine::scene
{
    /// \brief Class that traverses the scene graph to update the transform data.
    class TreeTraverser
    {
    public:
        explicit TreeTraverser(engine::Engine* engine);

        /// \brief Push a transform matrix to the stack.
        /// \param mat4 Transform matrix to push.
        void pushStack(glm::mat4 mat4);

        /// \brief Remove the last pushed transform matrix.
        /// \return Removed transform matrix in world space.
        glm::mat4 popStack();

        /// \brief Inititial traversal of the scene graph.
        ///
        /// Gets called by the scene subsystem.
        /// \param graph Scene graph to traverse.
        void initialTraversal(SceneGraph* graph);

        /// \brief Call initialVisit(..) on the next CTransform.
        /// \param component CTransform handle to initialVisit.
        void initNext(scene::components::transform_handle_t component);

        /// \brief Start the traversal of the scene.
        /// \param graph Scene graph to traverse.
        void traverseScene(SceneGraph* graph);

        /// \brief Call initialVisit(..) on the next CTransform.
        /// \param component CTransform to visit next.
        void visitNext(scene::components::transform_handle_t component);

        /// \brief Add a camera by providing the CTransform ID it is attached to.
        ///
        /// Cameras are used to automatically generate viewports.
        /// \param camera ID of the parent CTransform.
        /// \return Camera ID.
        int addCamera(const engine::components::id_t& camera);

        /// \brief Add a cinematic camera by providing the CTransform ID it is attached to.
        /// \param camera ID of the parent CTransform.
        /// \return Camera ID
        int addCinematicCamera(const engine::components::id_t& camera);

        void setEye(const glm::vec3& eye);

        void setView(const glm::mat4& view);

        void setProjection(const glm::mat4& projection);

        /// \brief Set skybox to use
        /// \param skybox
        void setSkybox(ITraversableObject* skybox);

        /// Switch rendering to or from cinematic rendering.
        /// \param RenderCinematic
        void setRenderCinematic(bool RenderCinematic);

        [[nodiscard]] bool shouldRenderCinematic() const;

        [[nodiscard]] bool isCinematicCameraPresent() const;

        [[nodiscard]] glm::vec3 getEye() const;

        [[nodiscard]] glm::mat4 getView() const;

        [[nodiscard]] glm::mat4 getProjection() const;

        [[nodiscard]] glm::mat4 getStackTop() const;

        [[nodiscard]] int getCurrentCameraID() const;

        [[nodiscard]] int getNumberOfViewports() const;

        [[nodiscard]] const scene::components::transform_handle_t& getCurrTransformComp() const;

    private:
        /// \brief Completly empty the transform stack.
        void emptyStack();

        /// \brief Creates a viewport for the given camera.
        /// \param cameraID ID of the camera to create the viewport for.
        void createViewport(int cameraID);

        /// \brief Traversing the scene graph path only to the given camera ID to set up matrices such as projection.
        /// \param cameraID ID of the camera to traverse to.
        void cameraTraversal(int cameraID);

        /// Start the traversal of the scene graph.
        /// \param graph Scene graph to traverse
        void traverseTree(const SceneGraph* graph);

        /// \brief Call the visit function to draw the skybox
        void renderSkybox();

        std::stack<glm::mat4> stack;

        engine::scene::components::transform_handle_t currTransformComp;

        glm::vec3 eye;

        glm::mat4 view;

        glm::mat4 projection;

        engine::Engine* engine;

        std::vector<scene::components::transform_handle_t> currentPath;

        std::map<int, std::vector<scene::components::transform_handle_t>> cameraPaths;

        int currentCameraID;

        int numberOfCameras;

        bool bIsCinematicCameraPresent;

        bool bRenderCinematic;

        int cinematicCameraID;

        engine::scene::ITraversableObject* skybox;
    };
}


