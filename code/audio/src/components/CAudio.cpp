//
// Created by CemOe on 23.01.2022.
//

#include <engine/audio/components/CAudio.h>
#include <engine/audio/Audio.h>
#include <cassert>

engine::audio::components::CAudio::CAudio(const engine::components::id_t id, const entities::id_t owner)
    : CComponent(id, owner), volume(1.f)
{

}

void engine::audio::components::CAudio::init(const engine::components::ConstructionInfo* constructionInfo)
{
    CComponent::init(constructionInfo);

    // Check if we have the right construction info
    auto* audioConstruction =
            dynamic_cast<const engine::audio::components::CAudio::ConstructionInfo*>(constructionInfo);
    assert(audioConstruction && "Please use the correct ConstructionInfo::CAudio::ConstructionInfo");

    pathToWav = audioConstruction->pathToWav;
    soundHandle = audioSystem->createSoundObject(pathToWav);
    volume = audioConstruction->volume < 0 ? volume : audioConstruction->volume;
}

void engine::audio::components::CAudio::play()
{
    audioSystem->play(soundHandle, volume);
}
