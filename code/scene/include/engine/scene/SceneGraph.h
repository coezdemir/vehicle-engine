//
// Created by CemOe on 15.08.2021.
//
#pragma once
#include <engine/components/CComponent.h>
#include <vector>

namespace engine::scene
{
    /// \brief Class holding the scene.
    class SceneGraph
    {
    public:
        friend class SceneSystem;
        friend class TreeTraverser;

        SceneGraph() = default;

        /// \brief Insert a CTransform to the scene
        ///
        /// Currently a CTransform gets automatically added to scene when its created in the scene subsystem
        /// \param component ID of the CTransform to insert
        void insert(engine::components::id_t component);

        /// \brief Remove a CTransform to the scene
        /// \param component ID of the CTransform to remove
        void remove(engine::components::id_t component);

    private:
        std::vector<engine::components::id_t> tree;
    };
}
