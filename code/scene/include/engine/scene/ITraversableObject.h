//
// Created by CemOe on 22.08.2021.
//
#pragma once
#include <functional>

namespace engine::scene
{
    /// \brief Interface to get information about the scene
    class ITraversableObject
    {
    public:
        /// \brief Gets called on the first traversal of scene graph at the beginning of the runtime.
        ///
        /// Call this by subscribing to the correct event in CTransform and when bInitialTraversed is false.
        /// \param traverser Traverser traversing the scene graph.
        virtual void initialVisit(class TreeTraverser* traverser) = 0;

        /// \brief Gets called every frame on traversal.
        ///
        /// Call this by subscribing to the correct event in CTransform.
        /// \param traverser Traverser traversing the scene graph.
        virtual void visit(class TreeTraverser* traverser) = 0;

    protected:
        bool bInitialTraversed = false;
    };
}