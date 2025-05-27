//
// Created by Cem on 16.07.2022.
//
#include "engine/renderer/manager/ModelManager.h"

std::map<engine::renderer::Model*, int, engine::renderer::manager::ModelManager::ModelCompare> engine::renderer::manager::ModelManager::models;
engine::renderer::manager::ModelLoader engine::renderer::manager::ModelManager::loader;

engine::renderer::manager::ModelHandle
engine::renderer::manager::ModelManager::loadModel(const std::filesystem::path& path, bool bIsParticleMesh)
{
    std::filesystem::path fullPath(MODELS_HOME);
    fullPath.concat(path.c_str());

    // Check if the model was already loaded
    {
        ModelHandle modelHandle = checkForModel(fullPath);

        if(modelHandle.isValid())
            return modelHandle;
    }

    auto it = models.emplace(new Model(), 1);

    loader.loadModel(fullPath, it.first->first, bIsParticleMesh);

    return ModelHandle(it.first->first);
}

engine::renderer::manager::ModelHandle
engine::renderer::manager::ModelManager::checkForModel(const std::filesystem::path& path)
{
    for (auto& entry : models)
    {
        if (entry.first->path.compare(path) == 0)
        {
            // Increment ref counter
            ++entry.second;

            auto modelHandle = ModelHandle(entry.first);

            return modelHandle;
        }
    }

    return {};
}

void engine::renderer::manager::ModelManager::addUser(ModelHandle handle)
{
    if(handle.isValid())
    {
        auto it = models.find(handle.model);

        if(it != models.end())
            it->second += 1;
    }
}

void engine::renderer::manager::ModelManager::lostUser(ModelHandle handle)
{
    if(handle.isValid())
    {
        auto it = models.find(handle.model);

        if(it != models.end())
        {
            it->second -= 1;
            if (it->second <= 0)
            {
                Model* modelToErase = it->first;
                models.erase(it);
                delete modelToErase;
            }
        }
    }
}