//
// Created by CemOe on 22.08.2021.
//
#include <engine/scene/SceneGraph.h>

void engine::scene::SceneGraph::insert(const engine::components::id_t component)
{
    tree.push_back(component);
}

void engine::scene::SceneGraph::remove(const engine::components::id_t component)
{
    for (int i = 0; i < tree.size(); ++i)
    {
        if(tree[i] == component)
        {
            tree[i] = tree.back();
            tree.pop_back();
            return;
        }
    }
}
